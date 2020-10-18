$origin_asm = get-ChildItem .\GZ2E01\ | select-object fullname,name
$dest_asm = get-ChildItem .\GZ2P01\ | select-object fullname,name
$origin_counter = 0
$origin_len = $origin_asm.length
$dest_len = $dest_asm.length
$counter = 0
for ($origin_counter = 0; $origin_counter -lt $origin_len; $origin_counter++) {
    Write-Output "File: $origin_counter/$origin_len"
    Write-Output "checking $($origin_asm[$origin_counter].fullname)"
    for ($dest_counter = 0; $dest_counter -lt $dest_len; $dest_counter++) {
        $diff = Compare-Object (Get-Content $origin_asm[$origin_counter].fullname) (Get-Content $dest_asm[$dest_counter].fullname)
        if ($diff.length -eq 0) {
            Write-Output "$($origin_asm[$origin_counter].fullname) matches $($dest_asm[$dest_counter].fullname)"
            $counter = 0
            break
        }
        $counter++;
        if ($counter -eq $origin_asm.length) {
            Write-Output "No match found for $($origin_asm[$origin_counter].fullname)"  | Out-file ./results.txt -Append
        }
    }
}

# foreach ($origin_asm[$origin_counter] in $origin_asm) {
#     $origin_counter++    
# }