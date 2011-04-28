#!/usr/bin/perl -w
#	-*- Perl -*-		DrupalInit.pl:	
#
#	(usage)% DrupalInit.pl
#
#	Inputs:		
#			
#	Outputs:		
#
#	David Wuertele	Thu Apr 28 11:34:48 2011	Steal This Program!!!

use strict;
use FindBin '$Bin';
use lib $Bin;
use DrupalConfig;
use Cwd 'abs_path';
use File::Basename;

my $quiet = undef;
my $dry_run = undef;

my $repository_relpath = shift;
if (! defined ($repository_relpath)) {
    die "usage:  $0 <repository_path>";
}

my $repository_path = abs_path ($repository_relpath);
my $repository_backup;

if (0) {

system_print ("mkdir -p $repository_path");
chdir $repository_path;
system_print ("git init");
system_print ("touch README");
system_print ("git add .");
system_print ("git commit -m 'initial commit'");

foreach my $branchname (keys %branch) {
    system_print ("git branch $branchname");
}

system_print ("git checkout drupal");

my %added_remote;
my @module_add_order;
@module_add_order = sort { ($a->{objects} || die ("$a->{remote} has no objects!")) <=> ($b->{objects} || die ("$b->{remote} has no objects!")) } @git_modules;
foreach my $module (@module_add_order) {
    if (! defined $added_remote{$module->{remote}}) {
#	$module->{url} =~ s/^http/git/;
	system_print ("git remote add -f $module->{remote} $module->{url}") == 0 or die "remote add failure: $?\n";
	if (defined $module->{tags}) { system_print ("git fetch --tags $module->{remote}") == 0 or die "remote tags failure: $?\n"; }
	$added_remote{$module->{remote}}++;
    }
}

chdir "$repository_path/.." or die "can't chdir to $repository_path/..: $!";
$repository_backup = "$repository_path" . ".remotes-added";
system_print ("rm -rf $repository_backup");
system_print ("cp -a $repository_path $repository_backup");
chdir $repository_path or die "can't chdir to $repository_path: $!";

my @module_merge_order = @git_modules;
foreach my $module (@module_merge_order) {
    system_print ("git merge -s ours --no-commit $module->{commit}");
    if (defined $module->{tree}) {
	system_print ("git read-tree --prefix=$module->{path} -u $module->{tree}");
    } else {
	system_print ("git read-tree --prefix=$module->{path} -u $module->{commit}");
    }
    system_print ("git commit -m 'Merged $module->{path}'");
}

chdir "$repository_path/.." or die "can't chdir to $repository_path/..: $!";
$repository_backup = "$repository_path" . ".gits-merged";
system_print ("rm -rf $repository_backup");
system_print ("cp -a $repository_path $repository_backup");
chdir $repository_path or die "can't chdir to $repository_path: $!";

}

# Modules distributed as files
my @module_install_order = @file_modules;
foreach my $module (@module_install_order) {
    chdir $repository_path or die "can't chdir to $repository_path: $!";
    system_print ("mkdir -p $module->{filename}.work");
    chdir "$repository_path/$module->{filename}.work" or die "can't chdir to $repository_path/$module->{filename}.work: $!";
    system_print ("wget $module->{url}");
    system_print ($module->{unarch});
    system_print ("rm -f $module->{filename}");
    chdir $repository_path or die "can't chdir to $repository_path: $!";
    system_print ("mkdir -p " . dirname($module->{path}));
    if (defined $module->{wrapper_dir}) {
	system_print ("mv $repository_path/$module->{filename}.work $repository_path/$module->{path}");
    } elsif (defined $module->{dirname}) {
	system_print ("mv $repository_path/$module->{filename}.work/$module->{dirname} $repository_path/$module->{path}");
    } else {
	system_print ("mv $repository_path/$module->{filename}.work/" . basename ($module->{filename}, qw(.zip .tar.gz .tar.bz2 .tbz)) . " $repository_path/$module->{path}");
    }
    system_print ("rm -rf $repository_path/$module->{filename}.work");
    system_print ("git add $module->{path}");
    system_print ("git commit -m $module->{url}");
}


chdir "$repository_path/.." or die "can't chdir to $repository_path/..: $!";
$repository_backup = "$repository_path" . ".files-merged";
system_print ("rm -rf $repository_backup");
system_print ("cp -a $repository_path $repository_backup");
chdir $repository_path or die "can't chdir to $repository_path: $!";
