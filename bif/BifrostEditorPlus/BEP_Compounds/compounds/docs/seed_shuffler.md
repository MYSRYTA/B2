Batch shuffles/randomizes graph parameters. This should not have any connections, but the position in the graph relevant to <span style="color:#e69963">scope</span>.

### <span style="color:#bb94ff">Shuffle</span>
Searches for and randomizes parameters based on the settings below.
<br />

### <span style="color:#bb94ff">Apply</span>
Applies listed values. For example if the previous shuffle gives an appealing result, the node could be duplicated as a 'checkpoint', allowing for further shuffling with the option to re-apply the 'checkpoint' results at any time.
<br />

### <span style="color:#bb94ff">Preview</span>
List the affected ports based on the current settings. The ports' current values are also listed. So in addition to previewing, this can also function as a way to store/restore the original values.
<br />

## Settings
### <span style="color:#e69963">scope</span>
Scope of the shuffler's effect.

- Local: only search the compound containing the shuffle node.

- Local Recursive: search compound containing the shuffle node, and recursively search all nested compounds.

- Global: search the entire graph.

### <span style="color:#d9be6c">search_terms</span>
Terms to search for ports with as a space separated list.
<br />

### <span style="color:#e69963">exact_match</span>
True, port names must exactly match one of the search terms. False, counts as a match if the port name in question contains one of the terms.
<br />

### <span style="color:#62cfd9">int_types</span>
Include int type ports in search. This includes similar types (long, uint, etc.) as well as vectors.
<br />

### <span style="color:#62cfd9">int_range</span>
Random value range for int type ports
<br />

### <span style="color:#82D99F">float_types</span>
Include float/double type ports in search. This includes vectors.
<br />

### <span style="color:#82D99F">float_range</span>
Random value range for float type ports
<br />

### <span style="color:#82D99F">float_precision</span>
How many decimal points to randomize float values with. This supports negative values, for example -2 would round to the nearest increment of 100.
<br />