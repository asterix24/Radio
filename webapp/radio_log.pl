use Mojolicious::Lite;
use POSIX qw(strftime);

my @measure_label = [
	"Data", "Ora",
	"Id", "LQI", "RSSI",
	"Up time",
	"T.CPU", "Vint",
	"T.1", "T.2"
];

my @dev0_map = (
	{"label" => "Id"       , "value" => "-" },
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

my @dev1_map = (
	{"label" => "Id"       , "value" => "-" },
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
	\@dev1_map,
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
	my %d = ();
	while (<FILE>)
	{
		chomp;
		$d{$1} = $_ if (/^(\d+);/);
	}
	close FILE;

	foreach (keys %d) {
		my @data = split ';', $d{$_};
		my $dev_id = $data[0];
		my $ref = $dev_map[$dev_id];

		#controllare se le lunghezze sono diverse..
		for my $i (0..$#data) {
			$ref->[$i]{'value'} = $data[$i];
		}
	}

	$self->stash(label => @measure_label);
	$self->stash(dev_map => \@dev_map);
	$self->stash(device => \%device);

} => 'index';

get '/test' => 'test';

app->start;



