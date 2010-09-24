my @MAN = map { "Documentation/$_.1" } qw(
fuzzydups
fuzzymunge
);

target;
priority 15;
dependon @MAN;
