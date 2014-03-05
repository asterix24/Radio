use Mojolicious::Lite;
use POSIX qw(strftime);

my @status_label = (
	"Id",
	"Data",
	"Ora",
	"LQI",
	"RSSI",
	"Up time",
);

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
	#$self->stash(host => $self->req->url->to_abs->host);
	#$self->stash(ua => $self->req->headers->user_agent);

	#Generate current file name
	my $n = strftime "%Y%m%d.log\n", localtime;
	print $n."\n";
	$n = "data/20140221.log";


	# Find lastest devices update.
	my %d = ();
	open(FILE, "<", $n) or die "cannot open < name: $!";
	while (<FILE>) {
		$d{$1} = $_ if (/^(\d+);/);
	}
	close FILE;

	# Fill data to render.
	my @status_dev = ();
	foreach (sort keys %d) {
		my @data = split ';', $d{$_};
		my $dev_id = $data[0];
		my $ref = $dev_map[$dev_id];
		my @d = ();

		for my $i (0..$#data) {
			#Break if we not have more label
			last if $i > $#{$ref};

			$ref->[$i]{'value'} = $data[$i];
			push @d, $data[$i] if $i <= $#status_label;
		}
		push @status_dev, [ @d ];
	}


	$self->stash(dev_map => \@dev_map);
	$self->stash(status_label => \@status_label);
	$self->stash(status_dev => \@status_dev);

} => 'index';

get '/test' => 'test';

app->start;



