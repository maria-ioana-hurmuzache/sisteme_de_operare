#!/bin/bash

if [ $# -eq 0 ]
then
	director="$HOME"
else
	director="$1"
fi

html_dir="$HOME/html"
music_dir="$html_dir/muzica"

#creez directoarele necesare daca acestea nu exista deja
if [ ! -d "$html_dir" ]
then
	mkdir "$html_dir"
fi

if [ ! -d "$music_dir" ]
then
	mkdir "$music_dir"
fi

playlist_file="$html_dir/playlist.html"

>"$playlist_file"
# ^ aici am scris cu un singur semn > pentru a redirectiona stdout spre fisier care va fi rescris sau creat daca acesta nu exista deja

find "$director" -type f -iname "*.mp3" | while read -r file
do
	file_name=$(basename "$file")
	ln "$file" "$music_dir/$file_name"
	#comanda : {file_name%.mp3} elimina terminatia .mp3 din numele file-ului
	echo "<a href=\"muzica/$file_name\">${file_name%.mp3}</a><br>" >> "$playlist_file"
	# ^aici scriu cu ">>" pentru a nu suprascrie continutul fisierului
done

chmod -R a+rx "$HOME" "$html_dir" "$music_dir"
chmod a+r "$playlist_file"

echo "am terminat"
