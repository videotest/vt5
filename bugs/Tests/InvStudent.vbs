' VBScript source code
Dim r, p
r=Array(1,2,3,4,5,10,30,100,30000)
p=Array(0.9, 0.95, 0.975, 0.995)
M_PI = 3.14159265358979323846

For Each ir in r
	Dim s
	s=""
	For Each jp in p
       s = s & ir & " " & jp & " " & InvStudentDistrib(ir, jp) & "|"
	Next
	WScript.Echo s
Next

Function InvStudentDistrib(ByVal r, ByVal p)
    Dim x1, x2, f1, f2, xc, fc, eps
    eps = 0.000001
    x1 = -100
    x2 = -x1
    f1 = StudentDistrib(r, x1)
    f2 = StudentDistrib(r, x2)
    While Abs(x2 - x1) > eps
        '            xc = x1 + (x2 - x1) * (p - f1) / (f2 - f1)
        xc = (x1 + x2) / 2.0
        fc = StudentDistrib(r, xc)
        If Sgn(p - f1) * Sgn(p - fc) > 0 Then
            x1 = xc
            f1 = fc
        Else
            x2 = xc
            f2 = fc
        End If
    WEnd
    InvStudentDistrib = xc
End Function

Function StudentDistrib(r, x)
    Dim z, eps
    eps = 0.0001
    z = r / (r + x * x)
    If (1 - z) > eps Then
        StudentDistrib = 0.5 + 0.5 * _
        (1.0 - BetaIncomplete(z, r / 2, 1 / 2) / Beta2(r)) * Sgn(x)
    Else
        StudentDistrib = 0.5
    End If
End Function

Function Beta2(ByVal r2)
    Dim Beta
    Dim r
    Dim i
    If (r2 Mod 2 = 0) Then
        Beta = 2.0
        r = CInt(r2 / 2.0)
        For i = r2 To 4 Step -2
            Beta = Beta * (i - 2) / (i - 1)
        Next
    Else
        Beta = 3.14159265358979323846
        r = CInt(r2 / 2.0)
        For i = r2 To 3 Step -2
            Beta = Beta * (i - 2) / (i - 1)
        Next
    End If
    Beta2 = Beta
End Function

Function BetaIncomplete(z,  a,  b)
    Dim sum
    sum = 0
    Dim factor
    Dim divider, dividend, n, summand, zPow
    n = 0
    zPow = 1.0
    dividend = 1.0
    divider = a
    summand = zPow * dividend / divider
    sum = summand
    Do
        n = n + 1
        zPow = zPow * z * (n - b) / n
        summand = zPow / (a + n)
        sum = sum + summand
    Loop Until summand < 0.000001
    BetaIncomplete = z ^ a * sum
End Function
