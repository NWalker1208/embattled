function _updateProgramOutput(result, outputElement) {
  if (result.length !== 0) {
    outputElement.innerHTML = "<b>Error</b><br>" + result;
    outputElement.classList.remove("success");
    outputElement.classList.add("error");
  } else {
    outputElement.innerHTML = "<b>Success!</b>";
    outputElement.classList.remove("error");
    outputElement.classList.add("success");
  }
}

onCanvasResize = Module.cwrap("OnCanvasResize", null, ["number", "number"])
_reloadAssemblyProgramA = Module.cwrap("ReloadAssemblyProgramA", "string", ["string"])
function reloadAssemblyProgramA(programStr, outputElement) {
  var result = _reloadAssemblyProgramA(programStr);
  _updateProgramOutput(result, outputElement);
}
_reloadAssemblyProgramB = Module.cwrap("ReloadAssemblyProgramB", "string", ["string"])
function reloadAssemblyProgramB(programStr, outputElement) {
  var result = _reloadAssemblyProgramB(programStr);
  _updateProgramOutput(result, outputElement);
}
