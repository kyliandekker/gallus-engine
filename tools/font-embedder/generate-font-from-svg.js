import fs from 'fs';
import svg2ttf from 'svg2ttf';

export function generateFontFromSvg(svgPath)
{
	return new Promise((resolve, reject) => {
		if (!svgPath)
		{
			reject('Font path is undefined or invalid.');
			return;
		}

		try
		{
			// Read the SVG font
			const svgContent = fs.readFileSync(svgPath, 'utf8');
			// Convert SVG to TTF
			const ttf = svg2ttf(svgContent, {});
			const ttfPath = svgPath.replace('.svg', '.ttf');
			fs.writeFileSync(ttfPath, Buffer.from(ttf.buffer));
			console.log('TTF font successfully created!');
			resolve({ fontPath: ttfPath });
		} catch (err)
		{
			console.error('Error creating TTF font:', err);
			reject(err);
		}
	});
}