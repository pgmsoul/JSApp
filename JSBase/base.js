function BViewParam() {
}
BViewParam.prototype.toString = function() {
    return "BViewParam";
}
BViewParam.prototype.addStyle = function(style) {
    this.style |= style;
}
BViewParam.prototype.delStyle = function(style) {
    this.style |= style;
    this.style -= style;
}
BViewParam.prototype.addExStyle = function(exStyle) {
    this.exStyle |= exStyle;
}
BViewParam.prototype.delExStyle = function(exStyle) {
    this.exStyle |= exStyle;
    this.exStyle -= exStyle;
}
function BMouse() {
}
BMouse.prototype.toString = function() {
    return "BMouse";
}
//gdi
function BColor(r,g,b,a) {
    if(r!==undefined) this.red = r;
    else this.red = 0;
    if(g!==undefined) this.green = g;
    else this.green = 0;
    if(b!==undefined) this.blue = b;
    else this.blue = 0;
    if(a!==undefined) this.alpha = a;
    else this.alpha = 0;
}
BColor.prototype.toString = function() {
    return "BColor";
}
BColor.prototype.getIntValue = function() {
    var v = this.red&0xFF;
    v += (this.green&0xFF << 8);
    v += (this.blue&0xFF << 16);
    v += (this.alpha&0xFF << 24);
}
BColor.prototype.setIntValue = function(v){
    this.red = v&0xFF;
    this.green = (v>>8)&0xFF;
    this.blue = (v>>16)&0xFF;
    this.alpha = (v>>24)&0xFF;
}
BFilePath = {};
BFilePath.getExt = function(path) {
    var i1 = path.lastIndexOf(".");
    var i2 = path.lastIndexOf("\\");
    var i3 = path.lastIndexOf("/");
    if (i1 < 0 || i2 > i1 || i3 > i1) return "";
    return path.substr(i1+1);
}
BFilePath.getName = function(path) {
    var i2 = path.lastIndexOf("\\");
    var i3 = path.lastIndexOf("/");
    var i = i2 > i3 ? i2 : i3;
    if (i < 0) return path;
    return path.substr(i + 1);
}
BFilePath.getTitle = function(path) {
    var name = BFilePath.getName(path);
    var i = name.lastIndexOf(".");
    if (i < 0) return name;
    return name.substr(0, i);
}
BFilePath.getParent = function(path) {
    var i2 = path.lastIndexOf("\\");
    var i3 = path.lastIndexOf("/");
    var i = i2 > i3 ? i2 : i3;
    if (i < 0) return "";
    return path.substr(0, i);
}
BFilePath.toSlash = function(path) {
    path = path.replace(/(\\{1,})|(\/{2,})/g,"/");
    if(path.length>0&&path.charAt(path.length-1)=="/") path = path.substr(0,path.length-1);
    return path;
}
BFilePath.toBackSlash = function(path) {
    path = path.replace(/(\\{2,})|(\/{1,})/g,"\\");
    if(path.length>0&&path.charAt(path.length-1)=="\\") path = path.substr(0,path.length-1);
    return path;
}
BFilePath.linkPath = function(path) {
    for(var i=1;i<arguments.length;i++)
        path += "/" + arguments[i];
    return BFilePath.toSlash(path);
}
//路径允许多个 "." 连续, 这个函数只处理ext的前面是否有一个 ".", 如果有多个点, 不继续处理.
BFilePath.linkExt = function(path,ext) {
    if(ext.length>0&&ext.charAt(0)!=".") ext = "." + ext;
    return path + ext;
}
BFilePath.isSame = function(path1, path2) {
    path1 = path1.toLowerCase();
    path2 = path2.toLowerCase();
    path1 = BFilePath.toSlash(path1);
    path2 = BFilePath.toSlash(path2);
    return path1.localeCompare(path2) == 0;
}
