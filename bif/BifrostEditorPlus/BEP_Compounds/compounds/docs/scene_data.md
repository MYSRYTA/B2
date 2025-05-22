Compound for conveniently writing data associated with the scene to a file.

### <span style="color:#bb94ff">Set Path</span>
Auto fills the parameters. For the file name the current scene path/name is used, this means multiple instances of this node in a give scene will all save to the same location. So unique key is set as well to allow multiple scene_data nodes to all read/write from the same file without overwriting each other.
<br />

### <span style="color:#bb94ff">Save</span>
Saves the current input. The input is stored as a property on a "master scene object" using the key as the property name.
<br />

### <span style="color:#bb94ff">Load</span>
This operation loads the master scene object from the file and stores it on a feedback port. This gives better performance as the file is not constantly being read, it also means changes to the file will not be loaded automatically.
<br />

## Settings

### <span style="color:#d9be6c">filename</span>
Path to save/load from.
<br />

### <span style="color:#d9be6c">key</span>
Key/property name to store input data within file.
<br />

### <span style="color:#e69963">mode</span>
Usually this is set automatically by the buttons. Set manually to override normal behavior.
<br />

### <span style="color:#e69963">reload</span>
Force the compound to always reload the file (when in read mode). Otherwise the file is only loaded when "Load" is pressed or feedback ports get reset.
<br />

## Outputs

### <span style="color:#90a3f4">output</span>
Depending on the mode this will simply be a pass-through of the input data, or the result of reading the file and associated key.
<br />

### <span style="color:#90a3f4">scene</span>
The entire scene object, ie. an object containing data from all other scene_data nodes. Data from a given node can be accessed from this port using a get_property node.
<br />