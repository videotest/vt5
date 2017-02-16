' VBScript source code
ActionManager.FileOpen "G:\bugs\tests\st1.image", "Image"
Data.SetValue "\StatUI\CalcForGroups", ""
Data.SetValue "\StatUI\CalcForAllClasses", ""
Data.SetValue "\StatUI\CalcForRelativeClasses", ""
CreateStatObject "T1", "St1"
CreateStatObject "T2", "St2"
CreateStatObject "T3", "St3"

pi = 3.14159265358979323846
eps = 0.0000001
set s1 = Objects.GetObjectByName ("St1")

St1_46 = s1.GetValue (0, 196646)
St1_46_all = s1.GetValue (-1, 196646)
St1_45 = s1.GetValue (-1, 196645)
St1_24 = s1.GetValue (0, 196624)
St1_39 = s1.GetValue (0, 196639)
St1_47 = s1.GetValue (0, 196647)
St1_48 = s1.GetValue (0, 196648)
St1_49 = s1.GetValue (0, 196649)
St1_50 = s1.GetValue (0, 196650)
St1_51 = s1.GetValue (0, 196651)
St1_51_all = s1.GetValue (-1, 196651)
St1_52 = s1.GetValue (0, 196652)

my_47 = 4* St1_46 / (pi * St1_45 )

if abs( my_47 - St1_47 ) > eps then
	MsgBox "для параметра 47 мое значения и Макса не совпадают, мое " & my_47 & " Макса: " & St1_47
end if

my_48 = pi * St1_24 / St1_46
my_48b = 4 * St1_39 / St1_47

if abs( my_48 - St1_48 ) > eps or abs(my_48b-my_48)>eps then
	MsgBox " St1_46="&St1_46&Chr(13)&Chr(13)& _
	" St1_24="&St1_24&Chr(13)& _
	"для параметра 48 мое значения и Макса не совпадают, мое " & my_48 &VbCr _
	& " Макса: " & St1_48 &VbCr _
	& " my_48b: " & my_48b &VbCr
end if
my_49 = 4 * ( 1 - St1_39 ) / St1_47
if abs( my_49 - St1_49 ) > eps then
	MsgBox " St1_47="&St1_47&Chr(13)&Chr(13)& _
	" St1_39="&St1_39&Chr(13)& _
	"для параметра 49 мое значения и Макса не совпадают, мое " & my_49 & " Макса: " & St1_49
end if

'error надо домнажать на  100
my_50 = St1_46 / St1_46_all * 100

if abs( my_50 - St1_50 ) > eps then
	MsgBox "для параметра 50 мое значения и Макса не совпадают, мое " & my_50 & " Макса: " & St1_50
end if

'error вместо St1_39 / St1_47 St1_47 / St1_39
my_51 = St1_47 / St1_39
if abs( my_51 - St1_51 ) > eps then
	MsgBox "для параметра 51 мое значения и Макса не совпадают, мое " & my_51 & " Макса: " & St1_51
end if

'error надо домнажать на  100
my_52 = St1_51 / St1_51_all * 100
if abs( my_52 - St1_52 ) > eps then
	MsgBox "для параметра 52 мое значения и Макса не совпадают, мое " & my_52 & " Макса: " & St1_52
end if
set s1 = Nothing

set s2 = Objects.GetObjectByName ("St2")

St2_46 = s2.GetValue (0, 196646)
St2_46_all = s2.GetValue (-1, 196646)
St2_45 = s2.GetValue (-1, 196645)
St2_24 = s2.GetValue (0, 196624)
St2_39 = s2.GetValue (0, 196639)
St2_47 = s2.GetValue (0, 196647)
St2_48 = s2.GetValue (0, 196648)
St2_49 = s2.GetValue (0, 196649)
St2_50 = s2.GetValue (0, 196650)
St2_51 = s2.GetValue (0, 196651)
St2_51_all = s2.GetValue (-1, 196651)
St2_52 = s2.GetValue (0, 196652)

'my2_47 = 4* St2_46 / (pi * St2_45 )
'my2_48 = pi * St2_24 / St2_46
'my2_49 = 4 * ( 1 - St2_39 ) / St2_47
'my2_50 = St2_46 / St2_46_all
'my2_51 = St2_39 / St2_47
'my2_52 = St2_51 / St2_51_all

my2_47 = 4* St2_46 / (pi * St2_45 )
my2_48 = pi * St2_24 / St2_46
my2_49 = 4 * ( 1 - St2_39 ) / St2_47
my2_50 = St2_46 / St2_46_all *100
my2_51 = St2_47 / St2_39
my2_52 = St2_51 / St2_51_all *100
my2_48b = 4 * St2_39 / St2_47

set s2 = Nothing

set s3 = Objects.GetObjectByName ("St3")

St3_46 = s3.GetValue (0, 196646)
St3_46_all = s3.GetValue (-1, 196646)
St3_45 = s3.GetValue (-1, 196645)
St3_24 = s3.GetValue (0, 196624)
St3_39 = s3.GetValue (0, 196639)
St3_47 = s3.GetValue (0, 196647)
St3_48 = s3.GetValue (0, 196648)
St3_49 = s3.GetValue (0, 196649)
St3_50 = s3.GetValue (0, 196650)
St3_51 = s3.GetValue (0, 196651)
St3_51_all = s3.GetValue (-1, 196651)
St3_52 = s3.GetValue (0, 196652)

'my3_47 = 4* St3_46 / (pi * St3_45 )
'my3_48 = pi * St3_24 / St3_46
'my3_49 = 4 * ( 1 - St3_39 ) / St3_47
'my3_50 = St3_46 / St3_46_all
'my3_51 = St3_39 / St3_47
'my3_52 = St3_51 / St3_51_all

my3_47 = 4* St3_46 / (pi * St3_45 )
my3_48 = pi * St3_24 / St3_46
my3_49 = 4 * ( 1 - St3_39 ) / St3_47
my3_50 = St3_46 / St3_46_all *100
my3_51 = St3_47 / St3_39
my3_52 = St3_51 / St3_51_all *100
my3_48b = 4 * St3_39 / St3_47


set s3 = Nothing

'Union47 = (my_47 + my3_47 + my3_47) / 3 
'Union48 = (my_48 + my3_48 + my3_48) / 3
'Union49 = (my_49 + my3_49 + my3_49) / 3
'Union50 = (my_50 + my3_50 + my3_50) / 3
'Union51 = (my_51 + my3_51 + my3_51) / 3
'Union52 = (my_52 + my3_52 + my3_52) / 3

Union47 = (my_47 + my2_47 + my3_47) / 3 
Union48 = (my_48 + my2_48 + my3_48) / 3
Union49 = (my_49 + my2_49 + my3_49) / 3
Union50 = (my_50 + my2_50 + my3_50) / 3
Union51 = (my_51 + my2_51 + my3_51) / 3
Union52 = (my_52 + my2_52 + my3_52) / 3
Union48b = (my_48b + my2_48b + my3_48b) / 3

MsgBox "For all"
CreateStatObject "t_all", "StAll"
set sM = Objects.GetObjectByName ("StAll")

M_Union47 = sM.GetValue (0, 196647)
M_Union48 = sM.GetValue (0, 196648)
M_Union49 = sM.GetValue (0, 196649)
M_Union50 = sM.GetValue (0, 196650)
M_Union51 = sM.GetValue (0, 196651)
M_Union52 = sM.GetValue (0, 196652)

set sM = Nothing

if abs( M_Union47 - Union47 ) > eps Then 
 MsgBox "( M_Union47 - Union47 ) > eps Мое значение: " &  Union47 & " Значение Макса: " & M_Union47
End If 

if abs( M_Union48 - Union48 ) > eps or abs(Union48 - Union48b)>eps Then 
 MsgBox _
	"St1_47=" & St1_47 & Chr(13)& _
	"St2_47=" &St2_47 & Chr(13)& _
	"St3_47=" &St3_47 &Chr(13)& _
	"my_48=" &my_48 & Chr(13)& _
	"my2_48=" &my2_48&Chr(13)& _
	"my3_48=" &my3_48&Chr(13)& _
	"my_48b=" &my_48b &VbTab &my_48b/3 &Chr(13)& _
	"my2_48b=" &my2_48b &VbTab &my2_48b/3 &Chr(13)& _
	"my3_48b=" &my3_48b &VbTab &my3_48b/3 &Chr(13)& _
	"St1_39=" & St1_39 & Chr(13)& _
	"St2_39=" &St2_39 & Chr(13)& _
	"St3_39=" &St3_39 &Chr(13)& _
	"( M_Union48 - Union48 ) > eps Мое значение: " & Union48 & " Значение Макса: " & M_Union48& _
	"Union48b= my_48b+my2_48b+my3_48b)/3 = " & Union48b
End If 

if abs( M_Union49 - Union49 ) > eps Then 
 MsgBox "St1_47="&St1_47&Chr(13)& _
" St2_47="&St2_47&Chr(13)& _
" St3_47="&St3_47&Chr(13)&Chr(13)&Chr(13)& _
" St1_39="&St1_39&Chr(13)& _
" St2_39="&St2_39&Chr(13)& _
" St3_39="&St3_39&Chr(13)&Chr(13)& _
" my_49="&my_49&Chr(13)& _
" my2_49="&my2_49&Chr(13)& _
" my3_49="&my3_49&Chr(13)& _
"( M_Union49 - Union49 ) > eps Мое значение: " &  Union49 & " Значение Макса: " & M_Union49
End If 

if abs( M_Union50 - Union50 ) > eps Then 
 MsgBox "( M_Union50 - Union50 ) > eps Мое значение: " &  Union50 & " Значение Макса: " & M_Union50
End If 

if abs( M_Union51 - Union51 ) > eps Then 
 MsgBox "( M_Union51 - Union51 ) > eps Мое значение: " &  Union51 & " Значение Макса: " & M_Union51
End If 

if abs( M_Union52 - Union52 ) > eps Then 
 MsgBox "( M_Union52 - Union52 ) > eps Мое значение: " &  Union52 & " Значение Макса: " & M_Union52
End If 


