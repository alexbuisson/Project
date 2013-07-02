link := "file:///"
pathrep := ""

CR := Chr(13)
LF := Chr(10)

EncodeURL( p_data, p_reserved=true, p_encode=true )
{
	old_FormatInteger := A_FormatInteger
	SetFormat, Integer, hex

	unsafe = 
		( Join LTrim
			25000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
			22233C3E5B5C5D5E607B7C7D7F808182838485868788898A8B8C8D8E8F9091929394
			95969798999A9B9C9D9E9FA0A1A2A3A4A5A6A7A8A9AAABACADAEAFB0B1B2B3B4B5B6
			B7B8B9BABBBCBDBEBFC0C1C2C3C4C5C6C7C8C9CACBCCCDCECFD0D1D2D3D4D5D6D7D8
			D9DADBDCDDDEDF7EE0E1E2E3E4E5E6E7E8E9EAEBECEDEEEFF0F1F2F3F4F5F6F7F8F9
			FAFBFCFDFEFF
		)
		
	if ( p_reserved )
		unsafe = %unsafe%24262B2C2F3A3B3D3F40
	
	if ( p_encode )
		loop, % StrLen( unsafe )//2
		{
			StringMid, token, unsafe, A_Index*2-1, 2
			StringReplace, p_data, p_data, % Chr( "0x" token ), `%%token%, all 
		}
	else
		loop, % StrLen( unsafe )//2
		{
			StringMid, token, unsafe, A_Index*2-1, 2
			StringReplace, p_data, p_data, `%%token%, % Chr( "0x" token ), all
		}
		
	SetFormat, Integer, %old_FormatInteger%

	return, p_data
}

GetUNCPath(letter)
{
  localName = %letter%
  length := 1000
  VarSetCapacity(remoteName, length)
  result := DllCall("Mpr\WNetGetConnection"
      , "Str", localName
      , "Str", remoteName
      , "UInt *", length)
  if (ErrorLevel <> 0 || result != 0)
  {
     return ""
  }
  else
  {
;     MsgBox, %remoteName% (%length%) -- result: %result%
    return remotename ; end function
  }
}

Loop Read, %1%
{
	p := A_LoopReadLine
	;MsgBox %p%
  if ( RegExMatch( p , "[A-Za-z][:]") ) ; match a regex for drive letter "^:"
  {
    unc := GetUNCPath( SubStr( p, 1, 2) )
    if ( unc <> "" )
    {
      withoutDriveLetter := SubStr( p, 3, strLen(p)-2 )
      pathrep = %link%%unc%%withoutDriveLetter%%CR%%LF%%pathrep%       
    }
  }
  else ; should already be an unc (check to be sure)
  {
    if ( RegExMatch( p , "\\\\") ) 
    {
      pathrep = %link%%p%%CR%%LF%%pathrep%
    }
    else
    {
      ; Msgbox "ignored " %p%
    }
  }  
}

StringReplace, pathrep, pathrep, \, /, All
StringReplace, pathrep, pathrep, %A_SPACE%, `%20 , All
StringReplace, pathrep, pathrep, ", , All
; MsgBox %pathrep%	
clipboard =  %pathrep% 

return