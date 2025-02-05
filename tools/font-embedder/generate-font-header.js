import inquirer from 'inquirer';
import { generateSvg } from './generate-svg.js';
import { generateFontFromSvg } from './generate-font-from-svg.js';
import { generateHtmlFromFont } from './generate-html-from-font.js';
import { generateHeaderFromFont } from './generate-header-from-font.js';

var name = 'IconFont';
var fontPath = 'C:/';
const glyphs = [];

await inquirer.prompt([
    {
		type: "input",
		name: "userInput",
		message: "Enter your font path:",
	},
]).then((answers) => {
	fontPath = answers.userInput;
});

await inquirer.prompt([
    {
		type: "input",
		name: "userInput",
		message: "Enter your font name:",
	},
]).then((answers) => {
	name = answers.userInput;
});

// Generate header file from font.
const { headerPath } = await generateHeaderFromFont(name, fontPath, glyphs);
console.log(`Header Path: "${headerPath}".`);