#!/usr/bin/perl -w
#	-*- Perl -*-		DrupalConfig.pm:	
#
#	Usage:
#		use DrupalConfig;
#
#	David Wuertele	Thu Apr 28 11:35:06 2011	Steal This Program!!!

package DrupalConfig;
use Exporter;
@ISA = ('Exporter');
@EXPORT = qw(@git_modules @file_modules %branch system_print);

$branch{drupal}{parent} = 'master';
$branch{patches}{parent} = 'drupal';
$branch{devel}{parent} = 'patches';
$branch{deploy}{parent} = 'devel';

# some shortcuts
local $drupal_project = "http://git.drupal.org/project";
local $modules_path = "drupal-6.x/sites/all/modules";

do "GitModules.pl";
do "FileModules.pl";

sub system_print {
    my @cmd = @_;

    my $started_at = time;
    print STDERR "Current time is $started_at\n";
    print STDERR join (" ", @cmd), "\n" if (! defined ($quiet));
    my $retval = system (@cmd) if (! defined ($dry_run));
    my $ended_at = time;
    my $duration = $ended_at - $started_at;
    print STDERR "Command took $duration seconds\n";
    return $retval;
}

1;
