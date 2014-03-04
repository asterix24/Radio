use Mojolicious::Lite;
use POSIX qw(strftime);

my @measure_label = [
	"Data", "Ora",
	"Id", "LQI", "RSSI",
	"Up time",
	"T.CPU", "Vint",
	"T.1", "T.2"
];

my @default_map = (
	"Data"     ,
	"Ora"      ,
	"LQI"      ,
	"RSSI"     ,
	"Up Time"  ,
	"CPU Temp" ,
	"CPU Vint"
);

my @dev0_map = (
	{"label" => "Data"     , "value" => "-" },
	{"label" => "Ora"      , "value" => "-" },
	{"label" => "LQI"      , "value" => "-" },
	{"label" => "RSSI"     , "value" => "-" },
	{"label" => "Up Time"  , "value" => "-" },
	{"label" => "CPU Temp" , "value" => "-" },
	{"label" => "CPU Vint" , "value" => "-" },
	{"label" => "Temp1"    , "value" => "-" },
	{"label" => "Temp2"    , "value" => "-" },
);

my @dev_map = (
	\@dev0_map,
	\@dev0_map,
);

# Simple plain text response
get '/' => sub {
	my $self  = shift;
	$self->stash(host => $self->req->url->to_abs->host);
	$self->stash(ua => $self->req->headers->user_agent);

	#Generate current file name
	my $n = strftime "%Y%m%d.log\n", localtime;
	print $n."\n";
	$n = "data/20140221.log";

	open(FILE, "<", $n) or die "cannot open < name: $!";
	my %device = ();
	while (<FILE>)
	{
		chomp;
		my @s = split ';', $_;
		$device{$s[0]} = \@s;
	}
	close FILE;

	$self->stash(label => @measure_label);
	$self->stash(dev_map => \@dev_map);
	$self->stash(device => \%device);

	print "....\n";
	foreach (@dev_map) {
		print "....qui...\n";
		foreach my $a ($_) {
			foreach my $h (@{$a}) {
				print ${$h}{'label'},${$h}{'value'},"\n";
			}
		}
		print "....\n";
	}

} => 'index';

get '/test' => 'test';

app->start;



