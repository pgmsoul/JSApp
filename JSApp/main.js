var dll = loadLibrary("JSBase.dll");
function main() {
    var f = new BFrame();
    var p1 = "\\asdfasd\\\\n\\";
    var p2 = "/AsdfaSd\\n";
    f.param.text = BFilePath.isSame(p1,p2);
    f.create();
    CMLStart();
}
