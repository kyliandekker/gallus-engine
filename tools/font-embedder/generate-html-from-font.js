import fs from 'fs';
import path from 'path';
import open from 'open';

export function generateHtmlFromFont(fontPath, glyphs)
{
	return new Promise((resolve, reject) => {
		if (!fontPath)
		{
			reject('Font path is undefined or invalid.');
			return;
		}

		if (!glyphs)
		{
			reject('Glyphs are undefined or invalid.');
			return;
		}

		try
		{
			const htmlPath = fontPath.replace('.ttf', '.html');
			const fontName = fontPath.replace(/^.*[\\/]/, '');
				
			let iconHTML = '';
			for (let i = 0; i < glyphs.length; i++) {
				const iconUnicode = glyphs[i].unicode;
				const iconName = glyphs[i].name;
				iconHTML += `\t\t<div class="icon ${iconName}">${iconUnicode}</div>\n`;
			}

			const htmlContent = `
<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Font Icon Demo</title>
		<style>
			@font-face {
				font-family: 'hello';
				src: url(${fontName}) format('truetype');
			}
			.icon {
				font-family: 'hello';
				font-size: 100px;
				margin: 10px;
			}
		</style>
	</head>
	<body>
		<h1>Icon Examples</h1>
${iconHTML}
	</body>
</html>
			`;

			fs.writeFileSync(htmlPath, htmlContent);
			console.log('HTML file successfully created!');

			// Open the HTML file in the default browser
			open(htmlPath).then(() => {
				console.log('HTML file opened in the default browser.');
				resolve({ htmlPath });
			}).catch(err => {
				console.error('Error opening HTML file:', err);
				reject(err);
			});
		} catch (err)
		{
			console.error('Error creating TTF font:', err);
			reject(err);
		}
	});
}