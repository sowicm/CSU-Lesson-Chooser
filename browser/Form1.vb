Imports System.Text
Public Class Form1
    Private Declare Function GetStdHandle Lib "kernel32" (ByVal nStdHandle As Long) As Long
    Private Declare Function WriteFile Lib "kernel32 " (ByVal hFile As Long, ByVal lpBuffer As String, ByVal nNumberOfBytesToWrite As Long, lpNumberOfBytesWritten As Long, ByVal lpOverlapped As Long) As Long
    Private hOut As Long
    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        Me.WindowState = 2
        WebBrowser1.Navigate(Command)
        hOut = GetStdHandle(-11)
    End Sub

    Private Sub WebBrowser1_DocumentCompleted(sender As Object, e As WebBrowserDocumentCompletedEventArgs) Handles WebBrowser1.DocumentCompleted
        Dim s As String
        s = e.Url.ToString()
        Dim dwWrite As Long
        'Dim bytes() As Byte
        'bytes = Encoding.Default.GetBytes(s)
        'MsgBox(s)
        WriteFile(hOut, s, s.Length(), dwWrite, 0)
        'MsgBox(WebBrowser1.StatusText)
        'MsgBox(e.Url.ToString())
    End Sub
End Class
