#!/usr/bin/perl 
use strict;
my (@lines,%result,$junk,$maxresults,$linkcolor,$odd,$even,$linecol);
my ($ip,$loyal,$harddisk,$ipho);
my (@arg,$alivecomp,@query,$minsize,$maxsize);
$maxresults=25;
print "Content-type: text/html\n\n";
print "<html><head><title>
	\"Tobu Search Results\"";
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

$inform{"search"}=~s/^\s+//;
chomp($inform{"search"});
$inform{"search"}=~s/\s+$//;
@query=split(/\s+/,$inform{"search"}); # what to search for
$arg[1]=$inform{"db_mp3"}; # whether to search db1 or not
$arg[2]=$inform{"db_video"};
$arg[3]=$inform{"db_others"};
$lines[1]=$inform{"lines_mp3"}; # lines seen of db1 
$lines[2]=$inform{"lines_video"};
$lines[3]=$inform{"lines_others"};
$alivecomp=$inform{"alivesearch"};
$ip=$ENV{"REMOTE_ADDR"};
my ($date,$mp3_c,$video_c,$others_c);
if($inform{"db_mp3"}) {$mp3_c="CHECKED";}
if($inform{"db_video"}) {$video_c="CHECKED";}
if($inform{"db_others"}) {$others_c="CHECKED";}
print "
</title></head>
<body bgcolor=white text=blue>

<form name=searchform  method=post action=\"http://10.144.0.1/cgi-bin/search.cgi\">
<TABLE CELLSPACING=0 CELLPADDING=0 BORDER=0 WIDTH=100%>
<TR>
	<TD><A HREF=\"http://10.156.3.207\"><IMAGE src=\"../icons/hbo.gif\" BORDER=0 height=100 width=90></A></TD>
	<TD ALIGN=CENTER><TABLE CELLSPACING=0 CELLPADDING=0 BORDER=0><TR>
	<TD ALIGN=CENTER>
	<input type=checkbox name=db_mp3 $mp3_c>&nbsp;Mp3 Songs&nbsp;&nbsp;&nbsp;&nbsp;
	<input type=checkbox name=db_video $video_c>&nbsp;Video files&nbsp;&nbsp;&nbsp;&nbsp;
	<input type=checkbox name=db_others $others_c>&nbsp;Other files</TD>
	
</TR>
<TR>
	<TD ALIGN=CENTER>
	<input type=text name=minsize size=3 value=$inform{\"minsize\"}>&nbsp;MB
	&nbsp;&nbsp;&nbsp;<<&nbsp;&nbsp;&nbsp;<input type=Text name=search size=30% value=\"$inform{\"search\"}\">&nbsp;&nbsp;&nbsp;<<&nbsp;&nbsp;&nbsp;
	<input type=text name=maxsize size=3 value=$inform{\"maxsize\"}>&nbsp;MB</TD>
<TD><input type=image src=\"../icons/tobusearch.jpg\" border=0 alt=\"Tobu Search\"></TD>
</TR>
</TD></TR></TABLE></TD>
<TD><A HREF=\"index.cgi\"><IMAGE src=\"../icons/home.gif\" BORDER=0></A></TD>
</TABLE>
<input type=hidden name=lines_mp3 value=0>
<input type=hidden name=lines_video value=0>
<input type=hidden name=lines_others value=0>
<input type=hidden name=alivesearch value=\"on\">
</FORM>";

# first check the validity of min and max size
if (!($inform{"minsize"}=~/^\s*\d*\.?\d*\s*$/)){
	print "<TR><TD>Error in Min Size : 
	<font color=red>$inform{\"minsize\"}</font><br></table>";
	print "</TR></TABLE>";
	exit;
}

if($inform{"minsize"}){	$minsize=$inform{"minsize"}*1024*1024 }
else{ $minsize=0; } # min size (bytes) disabled

if (!($inform{"maxsize"}=~/^\s*\d*\.?\d*\s*$/)){
	print "<TR><TD>Error in Max Size : 
	<font color=red>$inform{\"maxsize\"}</font><br></table>";
	print "</TR></TABLE>";
	exit;
}

if($inform{"maxsize"}){ $maxsize=$inform{"maxsize"}*1024*1024}
else{ $maxsize=0; } # max size (bytes) disabled

# now display what was searched
$junk=""; # junk will say what was searched
if($arg[1]){ $junk="mp3";}
if($arg[2]){
	if($junk){ $junk.=", videos"; }
	else { $junk="videos"; }
}
if($arg[3]){
	if($junk){ $junk.=", others"; }
	else { $junk="others"; }
}

print "<TR><TD><b><font color=red size=3>Error : You have not specified any database to search on</font></b>
	<TR><TD><b><font color=red size=2><BR>Choose atleast one of the three categories (mp3, videos and others) to begin search</font></b><BR>" unless ($junk);

if(-f "/nfs/home/tobu/files/loyal/$ip"){
	open(R,"/nfs/home/tobu/files/loyal/$ip");
	$loyal=<R>;
	chomp($loyal);
	close (R);
}
else{$loyal=0;}
print "<TR><TD><font color=darkblue>User from $ip and sharing $loyal MB of data</font><br>";

print "</table>
</TD>
</TR></TABLE>";
unless(-f "/nfs/home/tobu/files/free/$ip"){
		if($loyal<2048.0){
		print "<font color=red>Error </font>: To use services of Tobu, You have to share atleast 2 GB of your hard disk.
		
		 </body></html> ";
		exit;
	}
}

open(D,"date +%T_%D |");
$date=<D>;
chomp($date);
close(D);
# logging of keywords
if(($lines[1]==0) && ($lines[2]==0) && ($lines[3]==0)){
	open(W,">>/var/www/keyword_log");
	print W "$date $ip : <$inform{\"search\"}> in <$junk> with size <".&sizehuman($minsize).",".&sizehuman($maxsize).">\n";
	close W;
}


my ($cnt_mp3,$cnt_video,$cnt_others,$size,$line,$dispcount,$ip,$share,$path,@junk);
$cnt_mp3=$cnt_video=$cnt_others=$dispcount=0;

print "<center><table width=99%><tr>";
#print "<td width=15%><font color=brown><b>IP Address</b></font>";
print	"<td width=10%><font color=brown><b>Size</b></font>
	<td width=15%><font color=brown><b>Shared Folder</b></font>
	<td width=70%><font color=brown><b>Directory + Filename</b></font>";


for ($junk=0;$junk<=$#query;$junk++)
{
	if($query[$junk]=~/^[\*]?\.(\w+)/)
	{
		$query[$junk]="\\.".$1."\$";
	}
	elsif($query[$junk]=~/^(\w+)\*\.(\w+)$/)
	{
		$query[$junk]="\\\\".$1."[\\w|\\s]*\\\.".$2."\$";
	}
	else
	{
		$query[$junk]=~s/\*/\\w\*/g;
		$query[$junk]=~s/\./\\./g;
	}
		print " <FONT COLOR=LIGHTBLUE>$query[$junk]</FONT>";
}
 
if($arg[1]){
	&searchdb(\$cnt_mp3,"/nfs/home/tobu/mp3.db",1);
}
if($arg[2]){
	&searchdb(\$cnt_video,"/nfs/home/tobu/video.db",2);
}
if($arg[3]){
	&searchdb(\$cnt_others,"/nfs/home/tobu/others.db",3);
}
$odd="#f3eff4";
$even="#f1f2dc";
$linecol=$odd;
foreach $line(keys %result){
	$ip=${$result{$line}}[0];
	$share=${$result{$line}}[1];
	$size=${$result{$line}}[2];
	$linkcolor=${$result{$line}}[3];
	$path=${$result{$line}}[4];
	$junk=${$result{$line}}[5];
	print "<tr bgcolor=$linecol>";
		#<td>$ip
	print 	"<td>$size<td>$share<td>
		<a href=\"file:$line\" target=\"_\"><font color=$linkcolor>
		$path</font></a>";
	if($junk>0){
		print"&nbsp; <font color=SlateBlue>(and $junk other(s))</font>";
	}
	if($linecol eq $odd){ $linecol=$even; }
	else{ $linecol=$odd; }
}
		

sub searchdb{
	my ($cnt,$file,$num)=@_;
	my ($keyword,$flag);

	open(R,$file) || print "Can't search through $file<br>";
	while($$cnt<$lines[$num]){ ${$cnt}++; <R>; }
	while($line=<R>){
		$flag=1;
		last if ($dispcount >= $maxresults);
		${$cnt}++;
		# check if size is OK
		$line=~s/\[(\d+)\]//;
		$size=$1;
		next if ($size < $minsize); # min size
		next if (($maxsize!=0)&&($size > $maxsize)); # max size
		# check is keywords match
		foreach $keyword(@query){
			unless($line=~/$keyword/i){ $flag=0; last; }
		}
		next if $flag==0;
		$line=~s/^\s+//;
		chomp($line);
		$size=&sizehuman($size);
		@junk=split(/\\/,$line);
		$ip=$junk[2];
		# check if this ip is alive
		if(-f "/nfs/home/tobu/files/ping/$ip"){
			$linkcolor="green"; 
		}
		else{ $linkcolor="red"; }
		next if($alivecomp && ($linkcolor eq "red"));
		# so we have to print this line
		$share=$junk[3];
		$path=$junk[4];
		for($i=5;$i<=$#junk;$i++){
			$path.="\/$junk[$i]";
		}
		$line="\\\\$ip\\$share";
		for($i=4;$i<$#junk;$i++){
			$line.="\\$junk[$i]";
		}
		$line.="\\";
		if(exists($result{$line})){
			${$result{$line}}[5]++;
		}
		else{
			$dispcount++;
			$result{$line}=[$ip,$share,$size,$linkcolor,$path,0];
		}
	}
}

print "</table></center>";

if($dispcount==$maxresults)
{
	print "<form method=post action=\"http://10.144.0.1/cgi-bin/search.cgi\">
	<input type=hidden name=db_mp3 value=$inform{\"db_mp3\"}>
	<input type=hidden name=db_video value=$inform{\"db_video\"}>
	<input type=hidden name=db_others value=$inform{\"db_others\"}>
	<input type=hidden name=search value=\"$inform{\"search\"}\">
	<input type=hidden name=minsize value=$inform{\"minsize\"}>
	<input type=hidden name=maxsize value=$inform{\"maxsize\"}>
	<input type=hidden name=lines_mp3 value=$cnt_mp3>
	<input type=hidden name=lines_video value=$cnt_video>
	<input type=hidden name=lines_others value=$cnt_others>
	<input type=hidden name=alivesearch value=$alivecomp>
	<center>
	<input type=submit value=\"Next $maxresults\">
	</center>
	</form>";
}
print "<BR>
<FONT COLOR=\"darkblue\"><A HREF=\"comment.cgi\">Read What other people have to say</A>
<BR>	
Post your comments/suggestions/critisicm here.
<FORM METHOD=\"POST\" ACTION=\"comment.cgi\">
<CENTER>
<TEXTAREA NAME=\"comment\" ROWS=3 COLS=40></TEXTAREA>
<BR>
<INPUT NAME=\"Post Message\" TYPE=\"submit\" VALUE=\"Post Message\">
&nbsp;&nbsp;
</CENTER>
</FORM></body></html>";
exit;


sub sizehuman{
	my $size=$_[0];
	return ($_[0]) if($size==0);
	if($size<1024){ # less than 1K
		return "$size B";
	}
	if($size<1024*1024){ # less than 1M
		$size=sprintf("%.1f",$size/1024);
		return "$size KB";
	}
	$size=sprintf("%.1f",$size/(1024*1024));
	return "$size MB";
}
