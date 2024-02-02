
# �J�����g�f�B���N�g�����̂��ׂẴT�u�f�B���N�g������jpg�����png�t�@�C�����擾
$filesJpg = Get-ChildItem -Path . -Recurse -Include *.jpg
$filesPng = Get-ChildItem -Path . -Recurse -Include *.png

#.jpg��ϊ�
foreach($f in $filesJpg)
{
    Start-Process -FilePath TL2_TexConverter.exe -ArgumentList $f,"-ml 0" -Wait
}

#.png��ϊ�
foreach($f in $filesPng)
{
    Start-Process -FilePath TL2_TexConverter.exe -ArgumentList $f,"-ml 0" -Wait
}

pause