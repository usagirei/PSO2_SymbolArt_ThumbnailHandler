$source = "bin\"
$destination = "bin\SarThumbnailHandler.zip"

$dllx86 = [System.IO.Path]::Combine($source, "x86", "Release", "Pso2SarThumbnailHandler.x86.dll")
$dllx64 = [System.IO.Path]::Combine($source, "x64", "Release", "Pso2SarThumbnailHandler.x64.dll")
$util   = [System.IO.Path]::Combine($source, "x86", "Release", "SarConvert.exe")
$script = [System.IO.Path]::Combine("Shared", "install_remove.bat")

Compress-Archive -Path $dllx86 ,$dllx64,$script -DestinationPath $destination -Force