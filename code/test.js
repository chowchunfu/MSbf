counter = 0;
CPS = 0;

function fn(){
	const div1 = document.getElementById("div1");
	div1.textContent = "button clicked";
	
	counter = counter + 1;
	n.textContent = counter;

	if (counter == 1) {
		start = Date.now();
		clock()
	}
	
}

function clock(){
	end = Date.now();
	timetaken = end - start
	
	cps.textContent = "CPS: ".concat(Math.round((counter / timetaken*1000)*100)/100)
	if (counter < 20) {
		div2.textContent = "Timetaken: ".concat(timetaken/1000);
		setTimeout(clock,50)
	}
	
	
	
}
