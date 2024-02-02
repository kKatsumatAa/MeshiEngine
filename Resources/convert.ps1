
# カレントディレクトリ内のすべてのサブディレクトリ内のjpgおよびpngファイルを取得
$filesJpg = Get-ChildItem -Path . -Recurse -Include *.jpg
$filesPng = Get-ChildItem -Path . -Recurse -Include *.png

#.jpgを変換
foreach($f in $filesJpg)
{
    Start-Process -FilePath TL2_TexConverter.exe -ArgumentList $f,"-ml 0" -Wait
}

#.pngを変換
foreach($f in $filesPng)
{
    Start-Process -FilePath TL2_TexConverter.exe -ArgumentList $f,"-ml 0" -Wait
}

pause