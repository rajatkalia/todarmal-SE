#!/usr/bin/perl

# few lines below are cgi stuff ... no interference
my ($in,@inform,$i,$key,$val,%inform);
read(STDIN,$in,$ENV{"CONTENT_LENGTH"});
@inform =split(/&/,$in);
foreach $i (0..$#inform)
{
    $inform[$i]=~ s/\+/ /g;
    ($key,$val) = split(/=/,$inform[$i],2);
    $key=~ s/%(..)/pack("c",hex($1))/ge;
    $val=~ s/%(..)/pack("c",hex($1))/ge;
    $inform{$key} .="\0" if (defined($inform{$key}));
    $inform{$key} .=$val;
}
# lines above are cgi stuff 
$inform{"comment"}=~s/^\s+//;
$inform{"comment"}=~s/\s+$//;	
$inform{"comment"}=~s/\r\n|\n/<BR>/g;
$ipro=$ENV{"REMOTE_ADDR"};


print "Content-type: text/html\n\n";
print "
<HTML>
<HEAD>
<TITLE>
\"Tobu Users Comments\"
</TITLE>";

$invaliduser=0;
$user=$ENV{"REMOTE_ADDR"};
if($inform{"comment"})
{
	
	open(W,">>/var/www/comment_log2");
	$user=$ENV{"REMOTE_ADDR"};
	open(D,"date +%D_%T |");
	$date=<D>;
	chomp($date);
	close(D);
	print W "->= $date ";
	if($inform{"anonymous"})
	{
		print W "0.0.0.0 $user\n";
	}
	else{ 	print W "$user\n"; }
	print W "->: $inform{\"comment\"}\n";
	close(W);
print "
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=windows-1252\">
<META HTTP-EQUIV=\"Refresh\" CONTENT=\"0;URL=http://10.144.0.1/cgi-bin/comment.cgi\">
<BODY>
Please go to <A href=\"http://10.144.0.1/cgi-bin/comment.cgi\">http://10.144.0.1/cgi-bin/comment.cgi
</BODY>
</HEAD>
</html>";
exit();
}

print "
</HEAD>
<BODY BGCOLOR=white TEXT=blue>
<FONT SIZE=5><CENTER><B><U>
Tobu Search Engine Notice Board
</B></U></CENTER></FONT>
<TABLE WIDTH=100%>
<TR>
	<TD>
	<DIV ALIGN=LEFT>
	<FORM METHOD=\"post\" ACTION=\"comment.cgi\">
	<TEXTAREA NAME=\"comment\" ROWS=3 COLS=40></TEXTAREA>
	<BR><INPUT NAME=\"Post Message\" TYPE=\"submit\" VALUE=\"Post Message\">
	&nbsp;&nbsp;
	
	</FORM>
	</DIV>
	</TD>
	
	<TD>
	<DIV ALIGN=RIGHT>
	<A HREF=\"http://10.144.0.1/cgi-bin/index.cgi\">
	<IMAGE SRC=../icons/home.gif BORDER=0></A>
	</DIV>
	</TD>
</TABLE>
<TABLE WIDTH=100% BORDER=1>
<TR>
	<TD width=8%><CENTER>Date (dd/mm/yy)</CENTER></TD>
	<TD width=9%><CENTER>Time</CENTER></TD>
	<TD width=14%><CENTER>IP</CENTER></TD>
	<TD width=59%>Comment</TD>
</TR>";

open(R,"/var/www/comment_log2");
$comment="";
while(<R>)
{
	if(/^->=/)
	{
		if ($#store != -1)
		{
			 push(@store,$comment);
		}
		# this is date line
		/(\d+)\/(\d+)\/(\d+)/;
		$date="$2/$1/$3";
		/(\d+)\:(\d+)\:(\d+)/;
		$time="$1:$2:$3";
		/(\d+)\.(\d+)\.(\d+)\.(\d+)/;
		$ip="$1.$2.$3.$4";
				push(@store,[$date,$time,$ip]);
	}
	elsif (/^->:/)
	{
		s/^->:\s*//;
		s/\s+$//;
		$comment=$_;
	}
	else
	{
		$comment.=$_;
	}
}
push(@store,$comment);

for ($i=$#store-1;$i>=0;$i--)
{
	if ($i%2==1)
	{
		# this is a comment
		$comment=$store[$i];
	}
	else
	{
		# this array element has date, time and ip for above comment
		$date=$store[$i][0];
		$time=$store[$i][1];
		$ip=$store[$i][2];
		print "<TR><TD><CENTER>$date</CENTER></TD>
			<TD><CENTER>$time</CENTER></TD>
			<TD><CENTER>$ip</CENTER></TD>";
		print "<TD>&nbsp;<FONT COLOR=BROWN>$comment</FONT>";
		print "</TD></TR>";
	}
}

print "</TABLE></BODY></HTML>";
