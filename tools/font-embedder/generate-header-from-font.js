import fs from 'fs';
import path from 'path';
import open from 'open';

export function generateHeaderFromFont(name, fontPath, glyphs)
{
	return new Promise((resolve, reject) => {
		if (!fontPath)
		{
			reject('Font path is undefined or invalid.');
			return;
		}

		if (!glyphs || glyphs.length == 0)
		{
			console.log('No glyphs provided. Generating font without icon defines.');
		}

		try
		{
			const fontBuffer = fs.readFileSync(fontPath);

			const byteArray = Array.from(fontBuffer);
			const outputPath = fontPath.replace('.ttf', '.h');

			var glyphDefinitions = "";
			
			if (glyphs && glyphs.length > 0)
			{
				glyphDefinitions = '\n' + glyphs.map(glyph => {
					const glyphName = `ICON_${glyph.name.replace('-', '_').toUpperCase()}`;

					const utf8Bytes = Buffer.from(glyph.unicode[0], 'utf-8');
					const bytes = Array.from(utf8Bytes).map(byte => `\\x${byte.toString(16).padStart(2, '0')}`).join('');
					return `\tconstexpr char ${glyphName}[] = \"${bytes}\";`;
				}).join('\n') + '\n';
			}
			
			var cppHeader = `#pragma once

namespace font
{`;
			cppHeader += glyphDefinitions;
			cppHeader += `
	unsigned char ${name.toUpperCase()}[${byteArray.length}] = { 
		${byteArray.map(byte => `0x${byte.toString(16).padStart(2, '0')}`).join(', ')}
	};
}`;
			fs.writeFileSync(outputPath, cppHeader);

			console.log('Header file successfully created!');
			resolve({ headerPath: outputPath });
		} catch (err)
		{
			console.error('Error creating TTF font:', err);
			reject(err);
		}
	});
}