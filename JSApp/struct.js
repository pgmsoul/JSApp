function BTwin(x,y) {
    this.x = x;
    this.y = y;
}
BTwin.prototype.toString = function() {
    return "BTwin";
}
BTwin.prototype.move = function(dx, dy) {
    this.x += dx;
    this.y += dy;
}
BTwin.prototype.setValue = function(x, y) {
    this.x = x;
    this.y = y;
}
function BRect(left,top,right,bottom) {
    this.left = left;
    this.top = top;
    this.right = right;
    this.bottom = bottom;
}
BRect.prototype.toString = function() {
    return "BRect";
}
BRect.prototype.pointIn = function (pt) {
    if (this.left > pt.x)
        return false;
    if (this.right <= pt.x)
        return false;
    if (this.top > pt.y)
        return false;
    if (this.bottom <= pt.y)
        return false;
    return true;
}
BRect.prototype.inRect = function(x,y) {
    if (this.left > x)
        return false;
    if (this.right <= x)
        return false;
    if (this.top > y)
        return false;
    if (this.bottom <= y)
        return false;
    return true;
}
BRect.prototype.getLocation = function() {
    return new BTwin(this.left, this.top);
}
BRect.prototype.setLocation = function(x, y) {
    if (x !== undefined) {
        this.right = this.right - this.left + x;
        this.left = x;
    }
    if (y !== undefined) {
        this.bottom = this.bottom - this.top + y;
        this.top = y;
    }
}
BRect.prototype.getSize = function() {
    return new BTwin(this.right - this.left, this.bottom - this.top);
}
BRect.prototype.setSize = function(x, y) {
    if (x !== undefined) {
        this.right = this.left + x;
    }
    if (y !== undefined) {
        this.bottom = this.top + y;
    }
}
BRect.prototype.positive = function() {
    if (this.right < this.left) {
        var v = this.left;
        this.left = this.right;
        this.right = v;
    }
    if (this.bottom < this.top) {
        var v = this.top;
        this.top = this.bottom;
        this.bottom = v;
    }
}
