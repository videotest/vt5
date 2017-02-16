' VBScript source code
ActionManager.FileOpen "G:\Image\1.image", "Image"
Data.SetValue "\StatUI\CalcForGroups", ""
Data.SetValue "\StatUI\CalcForAllClasses", ""
Data.SetValue "\StatUI\CalcForRelativeClasses", ""
Data.SetValue "\StatUI\ChartParams\VisibleClass", ""
Data.SetValue "\StatUI\StatTable\VisibleClass", ""
Data.SetValue "\StatUI\ChartParams\Axis\X_ParamKey", 0
'''''''''''''''''''''''''''''''''''''''''''''
eps = 0.000001
my196656=0.000309351
my196618=0.000309351
FilterStatTable " Table1","TClass_0", 0, 0, "0"
FilterStatTable " Table1","TClass_3", 0, 0, "3"
FilterStatTable " Table1","TClass_4", 0, 0, "4"
CreateStatObject " Table1", "AllStat"
set AllSt = Objects.GetObjectByName ("AllStat")
	max196656 = AllSt.GetValue(0,196656)
	max196618 = AllSt.GetValue(0,196618)
set AllSt = Nothing
MsgBox max196656
MsgBox max196618
if abs( max196656 - my196656 ) > eps Then
	MsgBox "Не сошлось для 196656"
End If
if abs( max196618 - my196618 ) > eps Then
	MsgBox "Не сошлось для 196618"
End If