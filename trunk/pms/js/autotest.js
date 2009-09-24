/*
 * Copyright © 2008 Garaj
 * ja viem ze sa to da robit aj efektivnejsie :) optimalizaciu prenechavam buducim generaciam, hlavne na IE krepe
 * publikovane pod GNU GPL licenciou :D 
 */   

var questions = [
 ['Ktorý z nasledovných systémov videokamier nie je digitálny', 1, 'VHS', 'DV', 'DVD-RAM', 'DVCAM', 'Pozri <a href="rozdelenie.html" target="_blank">druhy kamier</a>.'],
 ['Ktorý z nasledovných technológií sa nepoužívajú v snímacích prvkoch kamier?', 3, 'CCD', 'CMOS', 'RFID', 'Pozri <a href="snimacieprvky.html" target="_blank">snímacie prvky</a>.'],
 ['Na koľko farebných kanálov sa rozkladá videoobraz pri kamerách vyššej triedy?', 2, 'štyri - CMYK', 'tri - RGB', 'päť - QWERT', 'Pozri <a href="snimacieprvky.html" target="_blank">snímacie prvky</a>.'], 
 ['Ktorý je najmodernejší typ videokamery?', 6, 'VHS', 'Betacam', 'PAL', 'SECAM', 'NTSC', 'HD', 'Pozri <a href="rozdelenie.html" target="_blank">druhy kamier</a>.'], 
 ['Ktorá z nasledovných firiem sa nepodielal na vývoji žiadnej technológie použitej v súčasných kamerách?', 1, 'Cementáreň Ladce', 'SONY', 'JVC', 'Kodak', 'Panasonic', 'Pozri <a href="predstavenie.html" target="_blank">kamera</a>.'],
 ['Kto je autorom týchto stránok?', 1, 'Ján Garaj', 'Dalajlama', 'Homer Simpson', 'Macko Uško', 'Wikipedia', 'Pozri <a href="ostranke.html" target="_blank">o stránke</a>.']
 ['Koľko obrázkov za sekundu musí snímať najmenej videokamera?', 1, '25', '27,5', '32', '64', 'Pozri <a href="index.html" target="_blank">motiváciu</a>.'],
 ['Ktoré z nasledovných súčiastok určite obsahuje kamera?', 1, 'objektív', 'čerpadlo', 'klaksón', 'rezervné koleso', 'tempomat', 'Pozri <a href="optickeprvky.html" target="_blank">optické prvky</a>.']
 
];

var gen_questions_array;
var gen_element_div;

function generate_quiz(in_questions, num_questions, delay, gen_element) {    
  var num = in_questions.size(); //num - pocet otazok vo vstupnom poli
  work_questions = in_questions.slice();;
  if((gen_element = document.getElementById(gen_element))==false) {
    alert('Nemam element kde budem generovat otazky!');
    return false;
  }
  gen_element_div = gen_element;
  if(num<num_questions) {
    alert('Nizky pocet otazok vo vstupnom poli!');
    gen_element.innerHTML = 'Chyba generovania otazok'; 
    return false;
  }
  var gen_questions = 0;  // gen_questions - pocet uz vygenerovanych otazok
  var to_generate = new Array; // pole kde sa budu nahodne ukladat otazky, ktore sa dalej vygeneruju do html
  var temp = null; //cislo
  while(gen_questions<num_questions) {
    //generuj otazku bo urobil si ich este malo
    temp = Math.floor(Math.random()*(work_questions.size()));
    to_generate.push(work_questions[temp]); //pridanie na generovanie
    work_questions.splice(temp ,1); //vyhodenie pouzitej otazky z moznych otazok
    gen_questions++;
  }  
  gen_element.innerHTML = '';
  gen_questions_array = to_generate; 
  draw_questions(delay);  
}



function draw_questions(delay) {
  var sizesid = gen_questions_array.size();
  var temp_question = gen_questions_array.pop();
  
  if(temp_question==null && sizesid==0) return false;
  if(temp_question==null) {
    setTimeout("draw_questions("+delay+")", delay);
    return true;
  }
  //kreslenie
  var otazka = temp_question[0];
  var spravna_odpoved = temp_question[1];
  var sizes = temp_question.size();
  var desc = temp_question[sizes-1];
  spravna_odpoved++;
  var gen_html = '';
  gen_html += '<div id="divul_'+sizesid+'" style="display: block;">\n<ul class="otazka">\n';
  gen_html += '<span>'+otazka+'</span><div>&nbsp;</div>\n';
  
    
  var temp_ids = new Array(); 
  for(i=2;i<sizes-1;i++) {
    temp_ids.push(i);
  }
  while(temp_ids.size()>0) {
    temp_index = Math.floor(Math.random()*(temp_ids.size()-1));
    temp = temp_ids[temp_index]; 
    temp_ids.splice(temp_index ,1);
    if(spravna_odpoved == temp) {
      gen_html += '<li><input type="radio" id="id_li_'+sizesid+'_'+temp+'" class="odpoved" name="otazka_0'+sizesid+'[]" value="1" /><label for="id_li_'+sizesid+'_'+temp+'">'+temp_question[temp]+'</label></li>\n';
    }
    else {
      gen_html += '<li><input type="radio" id="id_li_'+sizesid+'_'+temp+'" class="odpoved" name="otazka_0'+sizesid+'[]" value="0" /><label for="id_li_'+sizesid+'_'+temp+'">'+temp_question[temp]+'</label></li>\n';
    }
  }
  // add last string - description
  gen_html += '<li id="otazka_0'+sizesid+'[]" style="visibility: hidden;">'+desc+'</li>'; //<input type="radio" id="id_li_'+sizesid+'_'+temp+'" class="odpoved" name="otazka_0'+sizesid+'[]" value="0" /><label for="id_li_'+sizesid+'_'+temp+'">'+temp_question[temp]+'</label></li>\n';
  gen_html += '</ul>\n</div>\n';
  gen_element_div.innerHTML += gen_html;
  //zavolanie sameho seba neskor
  
  setTimeout("draw_questions("+delay+")", delay);
}



/**
 * Funkcia na ohodnotenie otazok z formulara
 *
 * frm - element formulara
 *
 * V html umiestnite formular, v ktorom budu jednotlive otazky v
 * v takomto formate:
 *
 * <form>
 *	<ul>
 *		Znenie otazky cislo 1:
 *		<li><input type="radio" class="odpoved" name="otazka_01[]" value="0" /><label>a) Odpoved 1<\/label><\/li>
 *		<li><input type="radio" class="odpoved" name="otazka_01[]" value="1" /><label>b) Odpoved 2<\/label><\/li>
 *		<li><input type="radio" class="odpoved" name="otazka_01[]" value="0" /><label>c) Odpoved 3<\/label><\/li>
 *		<li id="otazka_01" style="visibility: hidden;">Description</li> 
 *	<\/ul>
 *	-- // --
 *	<p>
 *		<input type="submit" value="potvrdit" onclick="return ohodnot(this.form);" />
 *	<\/p>
 * <\/form>
 */
function ohodnot(frm)
{
	// 
	// nazov class pomocou ktorej sa hladaju input elementy
	var MOZNOST_CLASS_NAME = 'odpoved';

	//
	// nazov triedy, ktorou bude oznaceny rodicovsky element spravnej odpovede
	var SPRAVNA_CLASS_NAME = 'spravna';
	
	//
	// nazov triedy, ktorou bude oznaceny rodicovsky element spravnej odpovede
	var NESPRAVNA_CLASS_NAME = 'nespravna';	


	// 
	// najdi vsetky odpovede
	var odpovede = document.getElementsByClassName( MOZNOST_CLASS_NAME, frm);
	
	//
	// ziskaj z nich unique zoznam "name"
	var nazvy = Array();
	odpovede.each(function (item) { 
		nazvy.push(item.name); 
	});
	nazvy = nazvy.uniq();
	
	// 
	// over ci su vsetky zaskrtnute
	for (var i=0; i<nazvy.length; i++) {
		var nazov = nazvy[i];
		
				
		// zoznam item v nich
		var moznosti = frm.getInputs('radio', nazov);
		
		// prelistuj ich a zisti ci je aspon 1 zaskrtnuty (viac nemoze byt, lebo maju rovnake meno)
		var zaskrtnute = false;
		var spravne_moznosti = 0;
		moznosti.each(function (moznost) {
			if (moznost.checked) {
				zaskrtnute = true;
			}
			if (moznost.value == 1) {	
				spravne_moznosti++;
			}
		});
		
		// ak nie je zakrtnuty, tak zrus validaciu
		if (!zaskrtnute) {
			alert('Musíte zodpovedať všetky otázky!');
			return false;
		}		
		document.getElementById(nazov).style.visibility = 'visible';
		//
		// ak otazka nema ani jednu spravnu odpoved v kode, alebo ma viac moznosti
		// upozorni autora
		if (spravne_moznosti != 1) {
			if (spravne_moznosti > 1) {
				alert('Otazka s parametrom name: '+ nazov + ' musi mat len 1 spravnu odpoved!');
			} else {
				alert('Otazka s parametrom name: '+ nazov + ' musi mat 1 spravnu odpoved !');
			}
			return false;
		}
		
	};
	
	//
	// statistiky vyhodnocovania
	var pocet_otazok = nazvy.length;
	var pocet_nespravnych = 0;
	var pocet_spravnych = 0;
	
	// 
	// vyhodnot jednotlive moznosti
	nazvy.each(function (nazov) {
		
		// zoznam item v nich
		var moznosti = frm.getInputs('radio', nazov);
		
		// prelistuj a ohodnot
		moznosti.each(function (moznost) {
			if (moznost.checked) {
				if (moznost.value == 1) {
					pocet_spravnych++;
				} else {
					pocet_nespravnych++;
				}
			}
		});
	});
	
	// 
	// zvyrazni spravne a nespravne vysledky
	nazvy.each(function (nazov) {
		
		// zoznam item v nich
		var moznosti = frm.getInputs('radio', nazov);
		
		// prelistuj a ohodnot
		moznosti.each(function (moznost) {
		
			// odstran predchadzajuce
			moznost.parentNode.removeClassName(SPRAVNA_CLASS_NAME);			
			moznost.parentNode.removeClassName(NESPRAVNA_CLASS_NAME);
			
			if (moznost.checked) {
				if (moznost.value == 1) {
					// spravna odpoved + celkovo spravna
					moznost.parentNode.addClassName(SPRAVNA_CLASS_NAME);
					moznost.parentNode.parentNode.removeClassName(NESPRAVNA_CLASS_NAME);
					moznost.parentNode.parentNode.removeClassName('otazka');
					moznost.parentNode.parentNode.addClassName('otazka'+SPRAVNA_CLASS_NAME);
					
				} else {
					// nespravna odpoved + celkovo nespravna
					moznost.parentNode.addClassName(NESPRAVNA_CLASS_NAME);
					moznost.parentNode.parentNode.removeClassName(SPRAVNA_CLASS_NAME);
					moznost.parentNode.parentNode.removeClassName('otazka');
					moznost.parentNode.parentNode.addClassName('otazka'+NESPRAVNA_CLASS_NAME);
					
				}
			} else {
				if (moznost.value == 1) {
					// spravna odpoved
					moznost.parentNode.addClassName(SPRAVNA_CLASS_NAME);
				}
			}
		});
	});
	
	//
	// zobraz vysledok za pomoci alert
	if (pocet_otazok == pocet_spravnych) {
		alert('Gratulujem !!!, zodpovedali ste všetky otázky správne!');
	} else {
		alert('Zodpovedali ste správne ' + pocet_spravnych + ' otázok z ' + pocet_otazok + ' možných.');
	}
	
	// 
	// allways return false - do not redirect
	return false;
}	