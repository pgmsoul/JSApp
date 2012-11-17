var dll = loadLibrary("JSBase.dll");
function main() {
    var search = new BFileSearch();
    search.onList = onList;
    var param = search.param;
    search.param = "abc";
    alert(search.param);
    param.childFolder = true;
    param.onlyFile = false;
    param.afterCall = true;
    search.filter.push("cpp","h","obj");
    search.filter = "abc";
    search.search("d:\\My Lib\\JavaScript\\App.JS\\JSApp");
}
function onList(inf) {
    output(inf.name);
}
