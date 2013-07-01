link := "file:///"
pathrep := ""

CR := Chr(13)
LF := Chr(10)

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
; MsgBox %pathrep%	
clipboard =  %pathrep% 
