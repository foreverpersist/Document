# Iptables

## Data flow

```
                            [INPUT] -> Local App  -> [OUTPUT]

                               ^                         |
                               |                         v

Entrance -> [PREROUTING] -> Routing -> [FORWARD] -> [POSTROUTING] -> Export
```

## Chain

	>* PREROUTING
	>* INPUT
	>* FORWARD
	>* OUTPUT
	>* POSTROUTING

## Table

### FILTER

	filt (ip)

	Chains:
		INPUT, FORWARD, OUTPUT

### NAT

	Nat (port map, address map)

	Chains:
		PREROUTING, POSTROUTING, OUTPUT[, INPUT]

### MANGLE

	Change data packets

	Chains:
		PREROUTING, POSTROUTING, INPUT, OUTPUT, FORWARD

### RAW

	NOTRACK

	Chains:
		PRETOUING, OUTPUT

### SECURITY

	MAC

## Target

### Default Targets

	> * ACEEPT	Accept data
	> * DROP	Drop data without response
	> * REJECT	Reject data with a reject response
	> * SNAT	Transfer source address
	> * MASQUERADE	One special case of SNAT
	> * DNAT	Transfer destination address
	> * REDIRECT	Do port map on local host
	> * LOG		Record in /var/log/messages
	> * ...

### Customized

	Defined by user

## Rule

	Rule is a tuple of (table, chain, match, target)
	In shell :
		iptables [-t table] command chain [match] [-j target]

	Default table is FILTER

	command:
		> * -P		Define default rule
		> * -A		Append a new rule
		> * -I num	Insert a new rule in index num
		> * -R num	Replace the rule in index num
		> * -D num	Delete the rule in index num

	match:
		> * -p <prot>	Match protocal (tcp, udp, icmp, ...)
		> * -s <ip>	Match source ip(s) format ip_begin[:ip_end]
		> * -d <ip>	Match destination ip(s)
		> * -i <net>	Match input net card (eth0, ...)
		> * -o <net>	Match output net card
		> * -dport <p>	Match destination port(s)
		> * -sport <p>	Match source port(s)
		> * -state <s>	Match connection state (NEW, ESTABLISHED, RELATED, ....)
		> * -m <m>	Extend rule


## Common Commands

### Default Local Security

```
$ iptables -P INPUT DROP
$ iptables -P OUTPUT ACCEPT
$ iptables -P FORWARD DROP
```

### Clear

```
$ iptables -P INPUT ACCEPT
$ iptables -F
$ iptables -X
$ iptables -Z
```

### Open Ports

```
$ iptables -A INPUT -p tcp --dport 8000:9000 -j ACCEPT
```

### Ping

	echo-request

```
$ iptables -A INPUT -p icmp -m icmp --icmp-type 8 -j ACCEPT
``

	echo-reply

```
$ iptables -A INPUT -p icmp -m icmp --icmp-type 0 -j ACCEPT
```
```

### Save & Restore Rules

```
$ iptables-save > /etc/iptables.up.rules
```

```
$ iptables-restore < /etc/iptables.up.rules
```
