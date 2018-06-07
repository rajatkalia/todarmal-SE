#!/usr/bin/perl
my ($total,$online,$ipro,@list,$ip);
$ipro=$ENV{"REMOTE_ADDR"};
if(-f "/nfs/home/tobu/files/names/$ipro")
{
open(R,"/nfs/home/tobu/files/names/$ipro");
$name=<R>; 
close(R);}
else
{$name="Unknown";}
open(R,"/nfs/home/tobu/files/loyal/$ipro");
$share=<R>; 
close(R);
if ($share eq "")
{
$share="0.0";
}
$ipho=substr($ipro,0,7);
if(($ipho eq "10.136.")||($ipho eq "10.140.")||($ipho eq "10.144.")||($ipho eq "10.148.")||($ipho eq "10.152.")||($ipho eq "10.156.")||($ipho eq "10.160.")||($ipho eq "10.164.")||($ipho eq "10.172."))
{
if($ipho eq "10.136.")
{
$hostel="SHIVALIK";
}
if($ipho eq "10.140.")
{
$hostel="VINDHYACHAL";
}
if($ipho eq "10.144.")
{
$hostel="KUMAON";
$mq="Tobu team members Vaibhav Mathur,Gaurav Arora and Kshitij Bansal stay back after the minors.for modifications";
}
if($ipho eq "10.148.")
{
$hostel="JWALAMUKHI"; 
}
if($ipho eq "10.152.")
{
$hostel="ARAVALI";
}
if($ipho eq "10.156.")
{
$hostel="KARAKORAM";
}
if($ipho eq "10.160.")
{
$hostel="NILGIRI";
}
if($ipho eq "10.164.")
{
$hostel="KAILASH";
}
if($ipho eq "10.172.")
{
$hostel="HIMADRI";
}
}
else
{
print"
<HTML>
<HEAD>
<META HTTP-EQUIV=\"Refresh\" CONTENT=\"0;URL=http://10.144.0.1/cgi-bin/comment.pl\">
</HEAD>
<BODY>
</BODY>
</HTML>
";
exit(0);
}
open(R,"/nfs/home/tobu/files/total");
$total=<R>; 
close(R);
$online=0;
$nonline=0;

opendir(DP,"/nfs/home/tobu/files/ping");
@list=readdir(DP);
closedir(DP);
foreach $ip(@list)
{
	next unless($ip=~/\d+\.\d+\.\d+\.\d+/);
	open(R,"/nfs/home/tobu/files/loyal/$ip");
	$part=<R>;
	$ipr=substr($ip,0,7);
	$nonline+=$part;
	if($ipr==$ipho)
	{$online+=$part;}
	close(R);
}
$nonline/=1024;
$online/=1024;
$nonline=sprintf("%.0f",$nonline);
$online=sprintf("%.0f",$online);
print "Content-type: text/html\n\n";
print "
<html>
<head> 
<title>Tobu Cycle</title>
</head>
<body bgcolor=white>

<TABLE BORDER=0 WIDTH=100%>
<TR>
<TD ALIGN=left>
<A HREF=\"http://10.144.0.1/cgi-bin/comment.pl\">
IIT Linux Users  <BR>Community<BR>
<IMAGE SRC=../icons/linux.gif BORDER=0 height=100 width=100>
</TD>
<TD ALIGN=CENTER>
	<img src=../icons/songanimate.gif>
</TD>
<TD ALIGN=RIGHT>
	<A HREF=\"http://10.144.0.1/cgi-bin/comment.cgi\">
	Click here for<BR>Notice Board<BR>
	<IMAGE SRC=../icons/noticeboard.jpg BORDER=0></A>
</TD>
</TR>
</TABLE>
<CENTER>
<b><font color=darkblue>Tracking $total GB of data with $nonline GB of online data<br>and your hostel $hostel sharing $online GB
</font></b><br>
<a href=http://10.144.0.1/win2k.html>Windows 2000/XP users</a>
<form name=searchform  method=post action=\"http://10.144.0.1/cgi-bin/search.cgi\">
<TABLE CELLSPACING=10 BORDER=0>
<TR>
	<TD></TD>
	<TD ALIGN=CENTER><input type=checkbox name=db_mp3>&nbsp;Mp3 Songs&nbsp;&nbsp;&nbsp;&nbsp;
	<input type=checkbox name=db_video>&nbsp;Video files&nbsp;&nbsp;&nbsp;&nbsp;
	<input type=checkbox name=db_others>&nbsp;Other files</TD>
	<TD></TD>
</TR>
<TR>
	<TD><input type=image src=\"../icons/search.jpg\" border=0 alt=\"Tobu Search\"></TD>
	<TD><input type=Text name=search size=55%></TD>
	<TD><input type=image src=\"../icons/tobusearch.jpg\" border=0 alt=\"Tobu Search\"></TD>
</TR>
<TR>
	<TD></TD>
	<TD ALIGN=CENTER><input type=text name=minsize size=3>&nbsp;MB
	&nbsp;&nbsp;&nbsp;<<&nbsp;&nbsp;&nbsp;<B>File Size</B>&nbsp;&nbsp;&nbsp;<<&nbsp;&nbsp;&nbsp;
	<input type=text name=maxsize size=3>&nbsp;MB</TD>
	<TD></TD>
</TR>
<TR>
	<TD></TD>
	<TD ALIGN=CENTER><input type=checkbox name=alivesearch CHECKED> Search Alive Comps only</TD>
	<TD></TD>
</TR>
</TABLE>
<input type=hidden name=lines_mp3 value=0>
<input type=hidden name=lines_video value=0>
<input type=hidden name=lines_others value=0>
</FORM>
<MARQUEE width=60%><font size=2 color=darkblue>$mq</font><MARQUEE>
</BODY>
</HTML>";





