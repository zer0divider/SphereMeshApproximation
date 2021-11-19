const PIXELS_PER_UNIT = 100;

function toPx(units){
	return PIXELS_PER_UNIT*units;
}

function vToPx(v){
	return {x:v.x*PIXELS_PER_UNIT, y:v.y*PIXELS_PER_UNIT};
}

function pxToV(x, y){
	return new Vec2(x/PIXELS_PER_UNIT, y/PIXELS_PER_UNIT);
}

class Vec2{
	constructor(x, y){
		this.x = x;
		this.y = y;
	}

	getNormalized(){
		let len = Math.sqrt(this.x*this.x + this.y*this.y);
		return new Vec2(this.x/len, this.y/len);
	}

	// get vector that is orthogonal to this one (rotated by 90 degrees CCW)
	getOrthogonal(){
		return new Vec2(this.y, -this.x);
	}

	// multiply with scalar
	mult(s){
		return new Vec2(this.x*s, this.y*s);
	}

	// add to this vector
	add(v){
		return new Vec2(this.x+v.x, this.y+v.y);
	}

	sub(v){
		return new Vec2(this.x-v.x, this.y-v.y);
	}

	dot(v){
		return this.x*v.x + this.y*v.y;
	}

	getLength(){
		return Math.sqrt(this.x*this.x + this.y*this.y);
	}
}


let _CTX = null;

class Plane{
	constructor(p, n, len=1){
		this.p = p;
		this.n = n.getNormalized();
		this.half_len = len/2;

		this.path_object = new Path2D();
		let v = this.n.getOrthogonal();
		let p1 = v.mult(this.half_len);
		let p2 = v.mult(-this.half_len);
		let p1_px = vToPx(p1);
		let p2_px = vToPx(p2);
		this.path_object.moveTo(p1_px.x, p1_px.y);
		this.path_object.lineTo(p2_px.x, p2_px.y);

		// normal path
		const normal_size = 0.5;
		this.normal_path = new Path2D();
		let p_px = vToPx(new Vec2(0,0));
		this.normal_path.moveTo(p_px.x, p_px.y);
		let n2 = this.n.mult(normal_size);
		let n2_px = vToPx(n2);
		this.normal_path.lineTo(n2_px.x, n2_px.y);

		// normal arrow
		const arrow_head_size = 0.1;
		this.normal_arrow_head = new Path2D();
		let arrow_head_n_start = this.n.mult(normal_size-arrow_head_size);
		let n_arrow1 = vToPx(arrow_head_n_start.add(v.mult(arrow_head_size)));
		let n_arrow2 = vToPx(arrow_head_n_start.add(v.mult(-arrow_head_size)));
		this.normal_arrow_head.moveTo(n_arrow1.x, n_arrow1.y);
		this.normal_arrow_head.lineTo(n2_px.x, n2_px.y);
		this.normal_arrow_head.lineTo(n_arrow2.x, n_arrow2.y);
	}

	setPosition(p){
		this.p = p;
	}

	getLength(){
		return this.half_len*2;
	}

	draw(){
		_CTX.save();
		let p_px = vToPx(this.p);
		_CTX.strokeStyle = "#202020";
		_CTX.translate(p_px.x, p_px.y);
		_CTX.lineWidth = 2;
		_CTX.stroke(this.path_object);
		_CTX.lineWidth = 1;
		_CTX.strokeStyle = "#808080";
		_CTX.stroke(this.normal_path);
		_CTX.stroke(this.normal_arrow_head);
		_CTX.restore();
	}
}

class RGBColor{
	constructor(r, g, b){
		this.r = r;
		this.g = g;
		this.b = b;
	}

	interpolate(other_color, t){
		if(t > 1){t = 1;}
		else if(t < 0){t = 0;}
		let r_dif = other_color.r - this.r;
		let g_dif = other_color.g - this.g;
		let b_dif = other_color.b - this.b;
		return new RGBColor(this.r + r_dif*t, this.g + g_dif*t, this.b + b_dif*t);
	}

	getRGBString(){
		return "rgb(" + this.r*255 + "," + this.g*255 + "," + this.b*255 + ")";
	}
	getRGBAString(alpha=1.0){
		return "rgba(" + this.r*255 + "," + this.g*255 + "," + this.b*255 + "," + alpha + ")";
	}
}

class Sphere{
	constructor(p, r){
		this.p = p;
		this.setRadius(r);
	}

	setPosition(p){
		this.p = p;
	}

	setRadius(r){
		this.r = r;
		let r_px = toPx(this.r);
		this.circle_path = new Path2D();
		this.circle_path.arc(0, 0, r_px, 0, Math.PI*2);
		this.radius_path = new Path2D();
		this.radius_path.moveTo(0,0);
		this.radius_path.lineTo(-r_px,0);
		this.color = new RGBColor(0,0,0);
	}

	getDistanceToPlane(p){
		return p.n.dot(p.p.sub(this.p)) - this.r;
	}

	getSquaredDistanceToPlane(p){
		let d = this.getDistanceToPlane(p);
		return d*d;
	}

	setColor(c){
		this.color = c;	
	}

	draw(){
		_CTX.save();
		let p_px = vToPx(this.p);
		_CTX.translate(p_px.x, p_px.y);

		_CTX.lineWidth = 2;
		_CTX.strokeStyle = this.color.getRGBString();
		_CTX.stroke(this.circle_path);

		_CTX.fillStyle = this.color.getRGBAString(0.5);
		_CTX.fill(this.circle_path);

		_CTX.lineWidth = 1;
		_CTX.strokeStyle = "#F0F0F0";
		_CTX.stroke(this.radius_path);

		_CTX.restore();
	}
}

let canvas = document.getElementById("sqem-canvas");

if(canvas.getContext){
	_CTX = canvas.getContext("2d");

	let unit_width = _CTX.canvas.width/PIXELS_PER_UNIT;
	let unit_height = _CTX.canvas.height/PIXELS_PER_UNIT;

	let RadiusSliderHTML = document.getElementById("sphere-radius-slider");
	let RadiusHTML = document.getElementById("sphere-radius");
	let NumPlanesHTML = document.getElementById("plane-number");
	let PlaneLengthWeightHTML = document.getElementById("plane-length-weight");
	function getSliderValue(){ return RadiusSliderHTML.value/10; }
	function setSliderValue(v){ RadiusSliderHTML.value = v*10; }
	let weigh_plane_length = NumPlanesHTML.value;

	let planes = [];
	let top_plane_pos = new Vec2(unit_width/2, unit_height/2 - 0.8);
	planes.push(new Plane(top_plane_pos, new Vec2(0.0, -1.0), 1));
	planes.push(new Plane(top_plane_pos.add(new Vec2(1.05, 0.55)), new Vec2(1.0, -1.0), 1.5));
	planes.push(new Plane(top_plane_pos.add(new Vec2(-1.05, 0.55)), new Vec2(-1.0, -1.0), 1.5));
	planes.push(new Plane(top_plane_pos.add(new Vec2(0, 1.1)), new Vec2(0.0, 1.0), 3.2));
	let num_planes = NumPlanesHTML.value;

	let sphere = new Sphere(new Vec2(unit_width/2, unit_height/2+0.2), getSliderValue());
	
	// sphere colors
	let color_high_error = new RGBColor(0.5, 0, 0);
	let color_low_error = new RGBColor(0, 0.9, 0.1);

	let currentQ = 0;

	_CTX.font = "24px sans";

	function draw(){
		// clear canvas
		_CTX.clearRect(0, 0, _CTX.canvas.width, _CTX.canvas.height);

		// draw planes
		for(let i = 0; i < num_planes; i++){
			planes[i].draw();
		}

		// draw sphere
		sphere.draw();

		// draw Q
		_CTX.fillText("Q(s) = "+currentQ.toFixed(3), 10, _CTX.canvas.height-10);
	}

	function updateQ(){
		currentQ = 0
		for(let i = 0; i < num_planes; i++){
			let q = sphere.getSquaredDistanceToPlane(planes[i]);
			if(weigh_plane_length){
				q *= planes[i].getLength();
			}
			currentQ += q;
		}
		sphere.setColor(color_low_error.interpolate(color_high_error, currentQ*2));
	}

	function updateSliderValue(){
		RadiusHTML.innerHTML = getSliderValue();
	}
	
	// adding events
	RadiusSliderHTML.addEventListener("input", (evt) => {
		updateSliderValue();
		sphere.setRadius(getSliderValue());
		updateQ();
		draw();
	});
	let mouseDrag = {startX:0, startY:0, spherePos:null, dragging:false}
	canvas.addEventListener("mousedown", (evt) => {
		mouseDrag.startX = evt.pageX - canvas.offsetLeft
		mouseDrag.startY = evt.pageY - canvas.offsetTop;
		let p = pxToV(mouseDrag.startX, mouseDrag.startY);
		if(p.sub(sphere.p).getLength() <= sphere.r){
			mouseDrag.dragging = true;
			mouseDrag.spherePos = sphere.p;
		}
	});
	canvas.addEventListener("mouseup", (evt) => {mouseDrag.dragging = false;});
	canvas.addEventListener("mousemove", (evt) => {
		if(mouseDrag.dragging){
			let delta = pxToV(	evt.pageX - canvas.offsetLeft - mouseDrag.startX,
								evt.pageY - canvas.offsetTop - mouseDrag.startY);
			sphere.setPosition(mouseDrag.spherePos.add(delta));
			updateQ();
			draw();
		}
	});

	canvas.addEventListener("wheel", (evt) => {
		let v = getSliderValue();
		if(evt.deltaY > 0){
			v -= 0.1;
		}else if(evt.deltaY < 0){
			v += 0.1;
		}
		if(v < 0.1){v = 0.1;}
		else if(v > 2.0){v = 2.0;}
		setSliderValue(v);
		updateSliderValue();
		sphere.setRadius(v);
		updateQ();
		draw();
	})

	NumPlanesHTML.addEventListener("change", (evt) =>{
		num_planes = evt.target.value;
		updateQ();
		draw();
	});
	PlaneLengthWeightHTML.addEventListener("change", (evt)=>{
		console.log("change: " + evt.target.checked);
		weigh_plane_length = evt.target.checked;
		updateQ();
		draw();
	});

	
	updateSliderValue();
	updateQ();
	draw();
}
else{
	let div = document.createElement("div");
	div.innerHTML = "<strong>Sorry</strong>, your browser doesn't support the 2D Canvas.";
	document.body.append(div);
}
