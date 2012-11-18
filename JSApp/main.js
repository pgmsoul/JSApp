var dll = loadLibrary("JSBase.dll");
function main() {
    var search = new BFileSearch();
    search.onList = onList;
    var param = search.param;
    //search.filter.push("cpp","h","obj");
    search.search("D:\\Video\\百家讲坛_两宋风云");
}
function onList(inf) {
    var src = inf.original + "/" + inf.name;
    var pos = inf.name.indexOf("两宋风云");
    if (pos < 0) return;
    pos += 4;
    inf.name = inf.name.substr(pos, 7) + ".rmvb";
    var dst = inf.original + "/" + inf.name;
    var r = BFileSystem.move(src, dst);
    output(inf.name + ":" + r);
}
