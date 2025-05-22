Similar to compute_on_frame, this compound helps improves graph performance by preventing the input from always re-computing.

### <span style="color:#bb94ff">Compute</span>
Recomputes the input.
<br />

### <span style="color:#e69963">pass</span>
When True this node acts as a simple pass through, ie. the input is always computed.
<br />

### <span style="color:#82D99F">trigger</span>
Changing this port's value is what triggers the input to be recomputed, this is all the 'Compute' button does. Alternatively this could be connected to another node so that changes to that node trigger a recompute.
<br />

### <span style="color:#82D99F">trigger_pass</span>
Pass through of the **trigger** port. This is intended to be connected to the triggers of other **compute_on_command** nodes, so that several can be triggered from a single node.
<br />