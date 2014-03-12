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

my @dev_0_map = (
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

my @dev_1_map = (
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

my @dev_2_map = (
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

my @dev_3_map = (
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

my @dev_map_available = (
	\@dev_0_map,
	\@dev_1_map,
	\@dev_2_map,
	\@dev_3_map,
);

# Simple plain text response
get '/' => sub {
	my $self  = shift;
	#$self->stash(host => $self->req->url->to_abs->host);
	#$self->stash(ua => $self->req->headers->user_agent);

	# Get the last log file
	my @log_file_list = ();
	my $dir =  "data";
	if (opendir(DIR, $dir)) {
		while (my $file = readdir(DIR)) {
			# Use a regular expression to ignore files beginning with a period
			next if ($file =~ m/^\./);
			push @log_file_list, $dir."/".$file;
		}
		closedir(DIR);
		@log_file_list = sort @log_file_list;
	}

	my %d = ();
	if (@log_file_list) {
		my $n = $log_file_list[$#log_file_list];
		print "current log file: $n\n";
		# Find lastest devices update.
		if (open(FILE, "<", $n)) {
			while (<FILE>) {
				my ($id, $val);
				# Save device id and all log data of it
				if (/^(\d+);/) {
					$id = $1;
					$val = $_;
				}
				#Check if current id is valid, and we have valid label map
				if ($id < $#dev_map_available) {
					$d{$id} = $val;
				} else {
					print "Error id=$id not valid, discard it! (max $#dev_map_available)\n";
					last;
				}
			}
			close FILE;
		} else {
			print "Error on $n: $!\n";
		}
	}

	# Fill data to render.
	my @dev_map = ();
	my @status_dev = ();
	foreach (sort keys %d) {
		my @data = split ';', $d{$_};
		my $dev_id = $data[0];
		my $ref = $dev_map_available[$dev_id];
		push @dev_map, $ref;
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

} => 'home';

get '/uno' => 'graph';

get '/json' => sub {
	my $self = shift;
	my @l = ();
	for (my $i = 0; $i < 24; $i++) {
		push @l, [ $i, rand(10)];
	}
	$self->render(json => [[@l]]);
};

app->start;



