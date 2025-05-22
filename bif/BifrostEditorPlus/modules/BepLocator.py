from builtins import range
import sys
import maya.api.OpenMaya as om
import maya.api.OpenMayaUI as omui
import maya.api.OpenMayaRender as omr
import maya.cmds as mc
import json


ZERO_POINT = om.MPoint(0, 0, 0)
UP_VECTOR = om.MVector(0, 1, 0)


def maya_useNewAPI():
	pass


class BepLocatorNode(omui.MPxLocatorNode):
	id = om.MTypeId(0x15c84e73)
	drawDbClassification = "drawdb/geometry/BepLocator_py"
	drawRegistrantId = "BepLocatorPlugin_py"
	size = None
	trans_attrs = [None] * 4
	rot_attrs = [None] * 4
	scale_attrs = [None] * 4
	matrix_attr = None
	args_attr = None
	shape_attr = None
	xray_attr = None
	label_attr = None
	fill_attr = None
	wire_attr = None
	width_attr = None
	pull_t_attr = None
	pull_r_attr = None
	pull_s_attr = None
	pull_m_attr = None
	fill_opacity_attr = None

	t_compute = None
	r_compute = None
	s_compute = None
	m_compute = None
	container = ''
	node_path = ''
	rotation_conversion = None

	editing = False

	@staticmethod
	def creator():
		return BepLocatorNode()

	@staticmethod
	def initialize():
		FnNum = om.MFnNumericAttribute()
		FnUnit = om.MFnUnitAttribute()
		FnTyped = om.MFnTypedAttribute()
		FnEnum = om.MFnEnumAttribute()

		BepLocatorNode.shape_attr = FnEnum.create("shape", "shp")
		FnEnum.addField("Box", 0)
		FnEnum.addField("Sphere", 1)
		FnEnum.addField("Sphere High", 2)
		FnEnum.addField("Point", 3)
		FnEnum.addField("Cross", 4)
		FnEnum.addField("Arrow Dynamic", 5)
		FnEnum.addField("Arrow X", 6)
		FnEnum.addField("Arrow Y", 7)
		FnEnum.addField("Arrow Z", 8)
		om.MPxNode.addAttribute(BepLocatorNode.shape_attr)

		BepLocatorNode.size = FnNum.create("size", "sz", om.MFnNumericData.kDouble, defaultValue=1)
		om.MPxNode.addAttribute(BepLocatorNode.size)

		BepLocatorNode.wire_attr = FnNum.create("wire", "wire", om.MFnNumericData.kBoolean, defaultValue=1)
		om.MPxNode.addAttribute(BepLocatorNode.wire_attr)

		BepLocatorNode.fill_attr = FnNum.create("fill", "fill", om.MFnNumericData.kBoolean, defaultValue=1)
		om.MPxNode.addAttribute(BepLocatorNode.fill_attr)

		BepLocatorNode.xray_attr = FnNum.create("xray", "xray", om.MFnNumericData.kBoolean, defaultValue=1)
		om.MPxNode.addAttribute(BepLocatorNode.xray_attr)

		BepLocatorNode.width_attr = FnNum.create("lineWidth", "lw", om.MFnNumericData.kDouble, defaultValue=1)
		om.MPxNode.addAttribute(BepLocatorNode.width_attr)

		BepLocatorNode.fill_opacity_attr = FnNum.create("fillOpacity", "fop", om.MFnNumericData.kDouble, defaultValue=0.2)
		om.MPxNode.addAttribute(BepLocatorNode.fill_opacity_attr)

		BepLocatorNode.label_attr = FnTyped.create('label', 'lb', om.MFnData.kString)
		om.MPxNode.addAttribute(BepLocatorNode.label_attr)

		BepLocatorNode.args_attr = FnTyped.create('arguments', 'args', om.MFnData.kString, om.MFnStringData().create("[]"))
		om.MPxNode.addAttribute(BepLocatorNode.args_attr)

		# Translate Attr
		tmp_list = [None] * 4
		bn_long = "sourceTranslate"
		bn_short = "srct"
		tmp_list[1] = FnNum.create(bn_long + "X", bn_short + "x", om.MFnNumericData.kDouble, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[1])
		tmp_list[2] = FnNum.create(bn_long + "Y", bn_short + "y", om.MFnNumericData.kDouble, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[2])
		tmp_list[3] = FnNum.create(bn_long + "Z", bn_short + "z", om.MFnNumericData.kDouble, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[3])
		tmp_list[0] = FnNum.create(bn_long, bn_short, tmp_list[1], tmp_list[2], tmp_list[3])
		FnNum.hidden = True
		om.MPxNode.addAttribute(tmp_list[0])
		BepLocatorNode.trans_attrs = tmp_list

		# Rotate Attr
		tmp_list = [None] * 4
		bn_long = "sourceRotate"
		bn_short = "srcr"
		tmp_list[1] = FnUnit.create(bn_long + "X", bn_short + "x", om.MFnUnitAttribute.kAngle, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[1])
		tmp_list[2] = FnUnit.create(bn_long + "Y", bn_short + "y", om.MFnUnitAttribute.kAngle, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[2])
		tmp_list[3] = FnUnit.create(bn_long + "Z", bn_short + "z", om.MFnUnitAttribute.kAngle, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[3])
		tmp_list[0] = FnNum.create(bn_long, bn_short, tmp_list[1], tmp_list[2], tmp_list[3])
		FnNum.hidden = True
		om.MPxNode.addAttribute(tmp_list[0])
		BepLocatorNode.rot_attrs = tmp_list

		# Scale Attr
		tmp_list = [None] * 4
		bn_long = "sourceScale"
		bn_short = "srcs"
		tmp_list[1] = FnNum.create(bn_long + "X", bn_short + "x", om.MFnNumericData.kDouble, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[1])
		tmp_list[2] = FnNum.create(bn_long + "Y", bn_short + "y", om.MFnNumericData.kDouble, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[2])
		tmp_list[3] = FnNum.create(bn_long + "Z", bn_short + "z", om.MFnNumericData.kDouble, defaultValue=0.0)
		om.MPxNode.addAttribute(tmp_list[3])
		tmp_list[0] = FnNum.create(bn_long, bn_short, tmp_list[1], tmp_list[2], tmp_list[3])
		FnNum.hidden = True
		om.MPxNode.addAttribute(tmp_list[0])
		BepLocatorNode.scale_attrs = tmp_list

		# Matrix Attr
		FnMatrix = om.MFnMatrixAttribute()
		BepLocatorNode.matrix_attr = FnMatrix.create("sourceMatrix", "srcm", om.MFnMatrixAttribute.kDouble)
		FnMatrix.hidden=True
		om.MPxNode.addAttribute(BepLocatorNode.matrix_attr)

		# Setup pull ports
		BepLocatorNode.pull_t_attr = FnNum.create("pullTranslate", "pullt", om.MFnNumericData.kChar, defaultValue=0)
		FnNum.hidden = True
		om.MPxNode.addAttribute(BepLocatorNode.pull_t_attr)
		BepLocatorNode.pull_r_attr = FnNum.create("pullRotate", "pullr", om.MFnNumericData.kChar, defaultValue=0)
		FnNum.hidden = True
		om.MPxNode.addAttribute(BepLocatorNode.pull_r_attr)
		BepLocatorNode.pull_s_attr = FnNum.create("pullScale", "pulls", om.MFnNumericData.kChar, defaultValue=0)
		FnNum.hidden = True
		om.MPxNode.addAttribute(BepLocatorNode.pull_s_attr)
		BepLocatorNode.pull_m_attr = FnNum.create("pullMatrix", "pullm", om.MFnNumericData.kChar, defaultValue=0)
		FnNum.hidden = True
		om.MPxNode.addAttribute(BepLocatorNode.pull_m_attr)

		om.MPxNode.attributeAffects(BepLocatorNode.trans_attrs[0], BepLocatorNode.pull_t_attr)
		om.MPxNode.attributeAffects(BepLocatorNode.rot_attrs[0], BepLocatorNode.pull_r_attr)
		om.MPxNode.attributeAffects(BepLocatorNode.scale_attrs[0], BepLocatorNode.pull_s_attr)
		om.MPxNode.attributeAffects(BepLocatorNode.matrix_attr, BepLocatorNode.pull_m_attr)

	def __init__(self):
		omui.MPxLocatorNode.__init__(self)
		self.inited_plugs = []

	def parseArgs(self):
		arg_str = om.MPlug(self.thisMObject(), self.args_attr).asString()
		arg_dict = json.loads(arg_str)
		if not arg_dict:
			return False

		self.container = arg_dict['container']
		self.node_path = arg_dict['node_path']
		self.t_compute = self.generateCompute(arg_dict['t_port'])
		self.r_compute = self.generateCompute(arg_dict['r_port'])
		self.s_compute = self.generateCompute(arg_dict['s_port'])
		self.m_compute = self.generateCompute(arg_dict['m_port'])
		self.rotation_conversion = self.generateRotateConversion(arg_dict['rotate_type'])

		return True

	def compute(self, plug, dataBlock):
		if not self.container:
			if not self.parseArgs():
				return

		if not self.editing:
			if mc.undoInfo(q=True, rn=True):
				mc.undoInfo(swf=False)

			elif plug.name() not in self.inited_plugs:
				self.inited_plugs.append(plug.name())
				mc.undoInfo(swf=False)

			else:
				self.editing = True
				mc.undoInfo(ock=True, cn="BepLocator_moved")
				mc.evalDeferred(self.finishedEditing)

		if plug == self.pull_t_attr:
			# print('compute t')
			value = dataBlock.inputValue(BepLocatorNode.trans_attrs[0]).asDouble3()
			self.t_compute(value)

		elif plug == self.pull_m_attr:
			# print('compute m')
			value = list(dataBlock.inputValue(BepLocatorNode.matrix_attr).asMatrix())
			self.m_compute(value)

		elif plug == self.pull_r_attr:
			# print('compute r')
			value = dataBlock.inputValue(BepLocatorNode.rot_attrs[0]).asDouble3()
			value = self.rotation_conversion(value)
			self.r_compute(value)

		elif plug == self.pull_s_attr:
			# print('compute s')
			value = dataBlock.inputValue(BepLocatorNode.scale_attrs[0]).asDouble3()
			self.s_compute(value)
		mc.undoInfo(swf=True)

	def finishedEditing(self):
		self.editing = False
		mc.undoInfo(cck=True, cn="BepLocator_moved")

	def isBounded(self):
		return True

	def isTransparent(self):
		return True

	def boundingBox(self):
		scale = om.MPlug(self.thisMObject(), BepLocatorNode.size).asDouble()
		corner1 = om.MPoint(scale, scale, scale)
		corner2 = om.MPoint(scale, scale, scale) * -1
		return om.MBoundingBox(corner1, corner2)

	@staticmethod
	def nullCompute(arg=None):
		# print('Attribute computed but has no destination')
		return arg

	def generateCompute(self, arg):
		arg_type = type(arg)
		if not arg:
			return self.nullCompute
		elif arg_type is str:
			return lambda value: self.sendAsVector(arg, value)
		elif arg_type is list:
			return lambda value: self.sendAsScalars(*arg, value)
		else:
			return self.nullCompute

	def sendAsVector(self, port, value):
		mc.vnnNode(self.container, self.node_path, spv=(port, '{' + str(value)[1:-1] + '}'))

	def sendAsScalars(self, x_port, y_port, z_port, value):
		if x_port:
			mc.vnnNode(self.container, self.node_path, spv=(x_port, str(value[0])))
		if y_port:
			mc.vnnNode(self.container, self.node_path, spv=(y_port, str(value[1])))
		if z_port:
			mc.vnnNode(self.container, self.node_path, spv=(z_port, str(value[2])))

	def generateRotateConversion(self, arg):
		if arg == 0:  # no conversion
			return lambda value: self.nullCompute(value)
		elif arg == 1:  # to degrees
			return lambda value: [item * 57.295779 for item in value]
		elif arg == 2:  # to quaternion
			return lambda value: om.MEulerRotation(value).asQuaternion()
		elif arg == 3:  # to X-Vector
			vector = om.MVector(1, 0, 0)
			return lambda value: vector.rotateBy(om.MEulerRotation(value))
		elif arg == 4:  # to Y-Vector
			vector = om.MVector(0, 1, 0)
			return lambda value: vector.rotateBy(om.MEulerRotation(value))
		elif arg == 5:  # to Y-Vector
			vector = om.MVector(0, 0, 1)
			return lambda value: vector.rotateBy(om.MEulerRotation(value))
		else:
			return lambda value: self.nullCompute(value)


# VP 2.0 Drawing ######################################################################

class BepLocatorDrawData(om.MUserData):
	def __init__(self):
		om.MUserData.__init__(self, False) # don't delete after draw
		self.shape_index = 1
		self.xray = False
		self.draw_color = om.MColor()
		self.draw_color_fill = om.MColor()
		self.size_scale = 1
		self.fLineList = om.MPointArray()
		self.fTriangleList = om.MPointArray()
		self.label = ""
		self.fill = True
		self.wire = True
		self.width = 1
		self.arrow = None


class BepLocatorDrawOverride(omr.MPxDrawOverride):
	@staticmethod
	def creator(obj):
		return BepLocatorDrawOverride(obj)

	def __init__(self, obj):
		omr.MPxDrawOverride.__init__(self, obj, None, False)
		self.mCustomBoxDraw = True
		self.mCurrentBoundingBox = om.MBoundingBox()

	def supportedDrawAPIs(self):
		return omr.MRenderer.kOpenGL | omr.MRenderer.kDirectX11 | omr.MRenderer.kOpenGLCoreProfile

	def hasUIDrawables(self):
		return True

	def isTransparent(self):
		return True

	def isBounded(self, objPath, cameraPath):
		return True

	def disableInternalBoundingBoxDraw(self):
		return self.mCustomBoxDraw

	def boundingBox(self, objPath, cameraPath):
		scale = om.MPlug(objPath.node(), BepLocatorNode.size).asDouble()
		corner = om.MPoint(scale, scale, scale)

		self.mCurrentBoundingBox.clear()
		self.mCurrentBoundingBox.expand(corner)
		self.mCurrentBoundingBox.expand(corner * -1)

		return self.mCurrentBoundingBox

	def prepareForDraw(self, objPath, cameraPath, frameContext, oldData):
		# Retrieve data cache (create if does not exist)
		data = oldData
		if not isinstance(data, BepLocatorDrawData):
			data = BepLocatorDrawData()

		node = objPath.node()
		data.shape_index = om.MPlug(node, BepLocatorNode.shape_attr).asInt()
		data.xray = om.MPlug(node, BepLocatorNode.xray_attr).asBool()
		data.fill = om.MPlug(node, BepLocatorNode.fill_attr).asBool()
		data.wire = om.MPlug(node, BepLocatorNode.wire_attr).asBool()
		data.size_scale = om.MPlug(node, BepLocatorNode.size).asDouble()
		data.width = om.MPlug(node, BepLocatorNode.width_attr).asDouble()
		data.draw_color = omr.MGeometryUtilities.wireframeColor(objPath)
		data.draw_color_fill = om.MColor(data.draw_color)
		data.draw_color_fill.a = om.MPlug(node, BepLocatorNode.fill_opacity_attr).asDouble()
		data.label = om.MPlug(node, BepLocatorNode.label_attr).asString()

		if data.shape_index == 5:
			data.arrow = om.MPlug(node, BepLocatorNode.trans_attrs[0]).asMDataHandle().asVector()

		# pull ports in draw function to force node to compute them
		om.MPlug(node, BepLocatorNode.pull_t_attr).asChar()
		om.MPlug(node, BepLocatorNode.pull_r_attr).asChar()
		om.MPlug(node, BepLocatorNode.pull_s_attr).asChar()
		om.MPlug(node, BepLocatorNode.pull_m_attr).asChar()

		return data

	def addUIDrawables(self, objPath, drawManager, frameContext, data):
		if not isinstance(data, BepLocatorDrawData):
			return

		drawManager.beginDrawable()
		drawManager.setColor(data.draw_color)
		drawManager.setLineWidth(data.width)
		if data.xray:
			drawManager.setDepthPriority(20000)
		else:
			drawManager.setDepthPriority(1)

		DRAW_FUNCTIONS[data.shape_index](drawManager, data)

		if data.label:
			drawManager.setColor(data.draw_color)
			drawManager.setFontSize( omr.MUIDrawManager.kDefaultFontSize)
			drawManager.text(ZERO_POINT, data.label, omr.MUIDrawManager.kCenter)

		drawManager.endDrawable()


def drawBox(drawManager, data):
	if data.wire:
		drawManager.box(ZERO_POINT, UP_VECTOR, om.MVector(0, 0, 1), data.size_scale, data.size_scale, data.size_scale)
	if data.fill:
		drawManager.setColor(data.draw_color_fill)
		drawManager.box(ZERO_POINT, UP_VECTOR, om.MVector(0, 0, 1), data.size_scale, data.size_scale, data.size_scale, filled=True)


def drawSphere(drawManager, data):
	if data.wire:
		drawManager.capsule(ZERO_POINT, UP_VECTOR, data.size_scale, 0, 6, 4)
	if data.fill:
		drawManager.setColor(data.draw_color_fill)
		drawManager.capsule(ZERO_POINT, UP_VECTOR, data.size_scale, 0, 6, 4, filled=True)


def drawSphereHigh(drawManager, data):
	if data.wire:
		drawManager.capsule(ZERO_POINT, UP_VECTOR, data.size_scale, 0, 20, 10)
	if data.fill:
		drawManager.setColor(data.draw_color_fill)
		drawManager.capsule(ZERO_POINT, UP_VECTOR, data.size_scale, 0, 20, 10, filled=True)


def drawPoint(drawManager, data):
	drawManager.setPointSize(8 * data.size_scale)
	drawManager.point(ZERO_POINT)


def drawCross(drawManager, data):
	drawManager.line(om.MPoint(data.size_scale, 0, 0), om.MPoint(-1 * data.size_scale, 0, 0))
	drawManager.line(om.MPoint(0, data.size_scale, 0), om.MPoint(0, -1 * data.size_scale, 0))
	drawManager.line(om.MPoint(0, 0, data.size_scale), om.MPoint(0, 0, -1 * data.size_scale))


def drawArrowDyn(drawManager, data):  # what is this redo this NOW *thunder sounds*
	point = om.MPoint(data.arrow)
	cone_height = data.size_scale * 0.25
	cone_base = om.MPoint(data.arrow.normal() * cone_height * -1)
	cyl_pos_vector = (point - cone_base * -1) * 0.5
	length = cyl_pos_vector.length() * 2
	radius = data.size_scale * 0.04
	if data.wire:
		drawManager.cylinder(om.MPoint(cyl_pos_vector - data.arrow), data.arrow, radius, length, 8)
		drawManager.cone(cone_base, data.arrow, cone_height * 0.5, cone_height)
	if data.fill:
		drawManager.setColor(data.draw_color_fill)
		drawManager.cylinder(om.MPoint(cyl_pos_vector - data.arrow), data.arrow, radius, length, 8, filled=True)
		drawManager.cone(cone_base, data.arrow, cone_height * 0.5, cone_height, filled=True)


def drawArrow(drawManager, data, point):
	cone_height = data.size_scale * 0.25
	cone_base = data.size_scale - cone_height
	cyl_radius = data.size_scale * 0.04
	cone_point = point * cone_base
	cyl_point = cone_point * 0.5
	dir = om.MVector(cone_point)
	
	if data.wire:
		drawManager.cone(cone_point, dir, cone_height * 0.5, cone_height)
		drawManager.cylinder(cyl_point, dir, cyl_radius, cone_base, 8)
	if data.fill:
		drawManager.setColor(data.draw_color_fill)
		drawManager.cone(cone_point, dir, cone_height * 0.5, cone_height, filled=True)
		drawManager.cylinder(cyl_point, dir, cyl_radius, cone_base, 8, filled=True)


DRAW_FUNCTIONS = (
	drawBox,
	drawSphere,
	drawSphereHigh,
	drawPoint,
	drawCross,
	drawArrowDyn,
	lambda x, y: drawArrow(x, y, om.MPoint(1,0,0)),
	lambda x, y: drawArrow(x, y, om.MPoint(0,1,0)),
	lambda x, y: drawArrow(x, y, om.MPoint(0,0,1))
)


# Plug-in ##############################################################################################################
def initializePlugin(obj):
	plugin = om.MFnPlugin(obj, "Hyuu", "BifrostEditor+ 2.3")

	try:
		plugin.registerNode("BepLocator", BepLocatorNode.id, BepLocatorNode.creator, BepLocatorNode.initialize, om.MPxNode.kLocatorNode, BepLocatorNode.drawDbClassification)
	except:
		sys.stderr.write("Failed to register node\n")
		raise

	try:
		omr.MDrawRegistry.registerDrawOverrideCreator(BepLocatorNode.drawDbClassification, BepLocatorNode.drawRegistrantId, BepLocatorDrawOverride.creator)
	except:
		sys.stderr.write("Failed to register override\n")
		raise


def uninitializePlugin(obj):
	plugin = om.MFnPlugin(obj)

	try:
		plugin.deregisterNode(BepLocatorNode.id)
	except:
		sys.stderr.write("Failed to deregister node\n")
		pass

	try:
		omr.MDrawRegistry.deregisterDrawOverrideCreator(BepLocatorNode.drawDbClassification, BepLocatorNode.drawRegistrantId)
	except:
		sys.stderr.write("Failed to deregister override\n")
		pass

