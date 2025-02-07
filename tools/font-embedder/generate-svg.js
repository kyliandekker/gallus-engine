import fs from 'fs';
import path from 'path';
import { SVGIcons2SVGFontStream } from 'svgicons2svgfont';

export function generateSvg(name)
{
	if (!name)
	{
		reject('SVG name is undefined or invalid.');
		return;
	}
	
	const __dirname = process.cwd();

	const fontStream = new SVGIcons2SVGFontStream({
		fontName: name,
		normalize: true,
		fontHeight: 1000, 
		descent: 200, 
	});

	const svgFolderPath = path.join(__dirname, 'svg');

	const svgFiles = fs.readdirSync(svgFolderPath).filter(file => file.endsWith('.svg'));

	const glyphs = [];

	const folderPath = path.join(__dirname, `${name}`);
	const outputPath = path.join(folderPath, `/${name}.svg`);

	if (!fs.existsSync(folderPath)) {
	  // Create the folder
	  fs.mkdirSync(folderPath, { recursive: true });
	}	

	const output = fs.createWriteStream(outputPath);
	fontStream.pipe(output);

	svgFiles.forEach((file, index) => {
		const iconPath = path.join(svgFolderPath, file);
	  
		const glyph = fs.createReadStream(iconPath);
		glyph.metadata = {
			unicode: [String.fromCharCode(0xE000 + index)],
			unicodeHTML: `\\${(0xE000 + index).toString(16).padStart(4, '0')}`,
			unicodeHex: `0x${(0xE000 + index).toString(16).toUpperCase()}`,
			name: file.replace('.svg', ''),
		};
		fontStream.write(glyph);
		glyphs.push(glyph);
	});

	fontStream.end();
	
	return new Promise((resolve, reject) => {
		// Return all glyphs we made.
		var glyphsInfo = [];
		for (let i = 0; i < glyphs.length; i++)
		{
			glyphsInfo.push(glyphs[i].metadata);
		}
		output.on('finish', () => {
			console.log('SVG successfully created!');
			resolve({ glyphs: glyphsInfo, svgPath: outputPath });
		});

		output.on('error', (err) => {
			console.error('Error creating SVG:', err);
			reject(err);
		});
	});
}