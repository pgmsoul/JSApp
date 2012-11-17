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
//获取字节数组
String.prototype.toBytes = function() {
    var result = [];
    for (var i = 0; i < this.length; i++)
        result.push(this.charCodeAt(i) >> 8, this.charCodeAt(i) & 0xff);
    return result;
}
//从字节数组生成, 注意JavaScript不能改变一个字串, 所以无法给this赋值, 返回值才是要的结果. 如果输入字节非偶数, 剩余的一个字节会被舍弃.
String.prototype.fromBytes = function(bytes) {
    var result = "";
    for (var i = 0; i < bytes.length; i += 2)
        result += String.fromCharCode((bytes[i] << 8) + bytes[i + 1]);
    return result;
}
//encode64编解码
global.encode64 = function(str) {
    var codeChar = "PaAwO65goUf7IK2vi9}xq8cFTEXLCDY1Hd3tV0ryzjbpN{BlnSs4mGRkQWMZJeuh";
    var s = "";
    var a = str.toBytes(); //取得字串的字节数组, 数组长度是字串长度的2倍.
    var res = a.length % 3; //3个字节一组进行处理, 余下特殊处理
    var i = 2, v;
    for (; i < a.length; i += 3) {//每3个字节用4个字符表示, 相当于3个字符(实际上是6个字节)用8个字符编码(实际为16个字节), 看起来容量膨胀了很多, 但是在启用压缩的情况下, 这些又被抵消掉了.
        v = a[i - 2] + (a[i - 1] << 8) + (a[i] << 16);
        s += codeChar.charAt(v & 0x3f);
        s += codeChar.charAt((v >> 6) & 0x3f);
        s += codeChar.charAt((v >> 12) & 0x3f);
        s += codeChar.charAt((v >> 18));
    }
    if (res == 1) {//字节余一位时候, 补2个字符, 64*64>256
        v = a[i - 2];
        s += codeChar.charAt(v & 0x3f);
        s += codeChar.charAt((v >> 6) & 0x3f);
    } else if (res == 2) {//字节余2位的时候, 补3个字节, 64*64*64>256*256, 所以是可行的.
        v = a[i - 2] + (a[i - 1] << 8);
        s += codeChar.charAt(v & 0x3f);
        s += codeChar.charAt((v >> 6) & 0x3f);
        s += codeChar.charAt((v >> 12) & 0x3f);
    }
    return s;
};
global.decode64 = function(codeStr) {
    var codeChar = "PaAwO65goUf7IK2vi9}xq8cFTEXLCDY1Hd3tV0ryzjbpN{BlnSs4mGRkQWMZJeuh";
    var dic = [];
    for (var i = 0; i < codeChar.length; i++) {
        dic[codeChar.charAt(i)] = i;
    }
    var code = [];
    var res = codeStr.length % 4;
    var i = 3, v;
    for (; i < codeStr.length; i += 4) {
        v = dic[codeStr.charAt(i - 3)];
        v += dic[codeStr.charAt(i - 2)] << 6;
        v += dic[codeStr.charAt(i - 1)] << 12;
        v += dic[codeStr.charAt(i)] << 18;
        code.push(v & 0xff, (v >> 8) & 0xff, (v >> 16) & 0xff);
    }
    if (res == 2) {//正确的字节数肯定是余2或3, 没有1的情况, 如果出现, 舍弃.
        v = dic[codeStr.charAt(i - 3)];
        v += dic[codeStr.charAt(i - 2)] << 6;
        code.push(v & 0xff);
    } else if (res == 3) {
        v = dic[codeStr.charAt(i - 3)];
        v += dic[codeStr.charAt(i - 2)] << 6;
        v += dic[codeStr.charAt(i - 1)] << 12;
        code.push(v & 0xff, (v >> 8) & 0xff);
    }
    return String.fromBytes(code);
};
    
