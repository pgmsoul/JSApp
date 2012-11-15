var pen,poly,brush;
var img;
function main(cmd) {
    var pen = new BPen();
    var f = new BFrame();
    var param = f.param;
    f.param = param;
    param.text = "BFrame";
	f.onMouse = onMouse;
	f.onCreate = onCreate;
	f.onClose = onClose;
	f.onDraw = onDraw;
	f.logPos("main");
	img = new BImage();
	img.load("DSCF4481.JPG");
	
	poly = new Array();
	poly[0] = new BTwin(10,10);
	poly[1] = new BTwin(200,200);
	poly[2] = new BTwin(100,200);
	f.create();
	var view;
	for(var i=0;i<0x1;i++){
	    view = new BEdit();
	    view.dispose();
	}
	view = new BEdit();
	view.create(f.handle());
	f.minSize = img.getSize();
	CMLStart();
	return 0;
}
function show2(str) {
    trace(str);
} function onDraw(dc) {
    var scr = this.scr();
    img.draw(dc,-scr.x,-scr.y);
}
function onClose(){
}
function onMouse(mouse) {
    if (mouse.event == BMouse.LBUTTONUP) {
        //this.size = new BTwin(600,600);
        //pen.color = 255;
        this.invalidate();
    } else if (mouse.event == BMouse.MOUSEMOVE) {
        this.text = mouse.x + "," + mouse.y;
    }
}
function onCreate(){
	//alert("onCreate");
}
CStartMsgPump();
