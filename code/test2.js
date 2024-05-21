const board = getCells()
const im = getCells()
const BombStat = {}

const WIDTH = 9
const HEIGHT = 9

function getCells(){
	Cells = {}
	for (let y= 0; y<9; y++){
		for (let x=0; x<9; x++){
			var Cell = [x,y];
			Cells[Cell] = 0
		}
	}
	return Cells
}


for (Cell of Object.keys(board)){
	pasteTile(Cell,"tile")
}


function pasteTile(Cell,filename){
	Cell = Cell.split(",")
	var rel_x = Cell[0] * 24
	var rel_y = Cell[1] * 24

	im[Cell] = document.createElement("img");
	im[Cell].id = "Cell" + Cell
	im[Cell].src = "bin/"+filename+".png";
	im[Cell].style = "position:absolute;left:"+rel_x+"px;top:"+rel_y+"px";
	im[Cell].addEventListener("click", function (){LclickTile(Cell)});

	
	src = document.getElementById("p");
	src.appendChild(im[Cell]);

}

function removeTile(Cell){
	src = document.getElementById("p");
	src.removeChild(im[Cell]);
}

function LclickTile(Cell){
	console.log(Cell);
	removeTile(Cell);
	pasteTile(Cell,"flag")
	
}

function generateBoard(){
	for (Cell of Object.keys(board)){
		if (Math.random() < 0.20625){
		board[Cell] = 1
		pasteTile(Cell,"flag")
		}
		else{
		board[Cell] = 0
		pasteTile(Cell,"tile")
		}
	}
}
function generateBoard_simplistic(){
	for (Cell of Object.keys(board)){
		if (Math.random() < 0.2){
		board[Cell] = 1
		}
		else{
		board[Cell] = 0
		}
	}
}
function countBombs(){
	Bombs = 0
	for (const [Cell,value] of Object.entries(board)){
		Bombs = Bombs + value
	}
	return Bombs
}

function generate1M(){
	startTime = Date.now()
	
	for (i=0;i<81;i++){
		BombStat[i] = 0
	}
	
	const report = document.getElementById("report");
	for (let i= 0; i<1000000; i++){
		generateBoard_simplistic();
		Bombs = countBombs();
		BombStat[Bombs]++;
		if (i % 10000 == 9999){
			report.textContent = "Generated "+(i+1)+" out of 10000 boards";
			console.log("Generated "+(i+1)+" out of 10000 boards");
		}
		
	}
	
	endTime = Date.now()
	timetaken = endTime - startTime
	const report2 = document.getElementById("report2");
	report2.textContent = "Timetaken: "+timetaken/1000;
	console.log("Timetaken: "+timetaken/1000)
	console.log(BombStat)
	
	BombString = convertToString(BombStat)
	downloadString(BombString,"CSV","BombString.csv")
}

function convertToString(a){
	let text = ""
	for ([key,value] of Object.entries(a)){
		text = text+key+","+value+"\n"
	}

	return text
}

function savetxt(){
	downloadString("abc", "text", "abc.txt")
}

function downloadString(text, fileType, fileName) {
  var blob = new Blob([text], { type: fileType });

  var a = document.createElement('a');
  a.download = fileName;
  a.href = URL.createObjectURL(blob);
  a.dataset.downloadurl = [fileType, a.download, a.href].join(':');
  a.style.display = "none";
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  setTimeout(function() { URL.revokeObjectURL(a.href); }, 1500);
}
