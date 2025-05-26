onCanvasResize = Module.cwrap("OnCanvasResize", null, ["number", "number"])
_reloadAssemblyProgramA = Module.cwrap("ReloadAssemblyProgramA", "string", ["string"])
function reloadAssemblyProgramA(programStr) {
  var result = _reloadAssemblyProgramA(programStr);
  if (result.length !== 0) { window.alert(result); }
}
_reloadAssemblyProgramB = Module.cwrap("ReloadAssemblyProgramB", "string", ["string"])
function reloadAssemblyProgramB(programStr) {
  var result = _reloadAssemblyProgramB(programStr);
  if (result.length !== 0) { window.alert(result); }
}
