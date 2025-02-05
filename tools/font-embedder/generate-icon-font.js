import inquirer from 'inquirer';
import { generateSvg } from './generate-svg.js';
import { generateFontFromSvg } from './generate-font-from-svg.js';
import { generateHtmlFromFont } from './generate-html-from-font.js';
import { generateHeaderFromFont } from './generate-header-from-font.js';

var name = 'IconFont';

await inquirer.prompt([
    {
		type: "input",
		name: "userInput",
		message: "Enter your font name:",
	},
]).then((answers) => {
	name = answers.userInput;
});

// Generate SVG font and get glyphs and font path
const { glyphs, svgPath } = await generateSvg(name);
console.log(`SVG Font Path: "${svgPath}".`);
console.log(`Loaded ${glyphs.length} icons.`);

// Generate font from SVG.
const { fontPath } = await generateFontFromSvg(svgPath);
console.log(`Font Path: "${fontPath}".`);

// Generate html from font (optional).
const { htmlPath } = await generateHtmlFromFont(fontPath, glyphs);
console.log(`Html Path: "${htmlPath}".`);

// Generate header file from font.
const { headerPath } = await generateHeaderFromFont(name, fontPath, glyphs);
console.log(`Header Path: "${headerPath}".`);