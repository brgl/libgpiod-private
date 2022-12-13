#!/bin/bash
# SPDX-License-Identifier: GPL-2.0-or-later
# SPDX-FileCopyrightText: 2023-2024 Bartosz Golaszewski <bartosz.golaszewski@linaro.org>

#
# Test cases for gpiocli utility. This test-suite assumes that gpio-manager
# is already running.
#

SOURCE_DIR="$(dirname ${BASH_SOURCE[0]})"

wait_for_sim() {
	COUNTER=100
	while true
	do
		gdbus call --system --dest io.gpiod1 \
			--object-path /io/gpiod1/chips/$1 \
			--method org.freedesktop.DBus.Peer.Ping > /dev/null 2>&1 && break
		sleep 0.01
		COUNTER=$(expr $COUNTER - 1)
		if [ $COUNTER -eq 0 ]
		then
			fail "error waiting for the GPIO sim chip to be exported on the bus"
			return 1
		fi
	done
}

# Create a simulated GPIO chip and wait until it's exported by the gpio-manager.
gpiosim_chip_dbus() {
	gpiosim_chip $*
	wait_for_sim ${GPIOSIM_CHIP_NAME[$1]}
}

gpiodbus_release_request() {
	run_prog gpiocli release $1
	status_is 0
}

gpiodbus_check_request() {
	run_prog gpiocli requests
	status_is 0
	num_lines_is 1
	output_regex_match $1
}

#
# gpiocli wait test cases
#

test_wait_for_manager() {
	run_prog gpiocli wait
	status_is 0
	num_lines_is 0
}

test_wait_for_chip() {
	dut_run gpiocli wait --chip=foobar
	sleep 0.01

	gpiosim_chip_dbus sim0 label=foobar
	dut_flush
	dut_read

	status_is 0
	num_lines_is 0
}

test_wait_timeout() {
	run_prog gpiocli wait --chip=foobar --timeout=100ms
	status_is 1
	num_lines_is 1
	output_regex_match ".*: wait timed out!"
}

#
# gpiocli detect test cases
#

test_detect_all_chips() {
	gpiosim_chip_dbus sim0 num_lines=4
	gpiosim_chip_dbus sim1 num_lines=8
	gpiosim_chip_dbus sim2 num_lines=16

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}
	local sim1=${GPIOSIM_CHIP_NAME[sim1]}
	local sim2=${GPIOSIM_CHIP_NAME[sim2]}
	local sim0dev=${GPIOSIM_DEV_NAME[sim0]}
	local sim1dev=${GPIOSIM_DEV_NAME[sim1]}
	local sim2dev=${GPIOSIM_DEV_NAME[sim2]}

	run_prog gpiocli detect

	status_is 0
	output_contains_line "$sim0 [${sim0dev}-node0] (4 lines)"
	output_contains_line "$sim1 [${sim1dev}-node0] (8 lines)"
	output_contains_line "$sim2 [${sim2dev}-node0] (16 lines)"

	# ignoring symlinks
	local initial_output=$output
	gpiosim_chip_symlink sim1 /dev

	run_prog gpiocli detect

	status_is 0
	output_is "$initial_output"
}

test_detect_one_chip() {
	gpiosim_chip_dbus sim0 num_lines=4
	gpiosim_chip_dbus sim1 num_lines=8
	gpiosim_chip_dbus sim2 num_lines=16

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}
	local sim2=${GPIOSIM_CHIP_NAME[sim2]}
	local sim0dev=${GPIOSIM_DEV_NAME[sim0]}
	local sim2dev=${GPIOSIM_DEV_NAME[sim2]}

	# by name
	run_prog gpiocli detect $sim0

	status_is 0
	output_contains_line "$sim0 [${sim0dev}-node0] (4 lines)"
	num_lines_is 1

	# by number
	run_prog gpiocli detect $(gpiosim_chip_number sim2)

	status_is 0
	output_contains_line "$sim2 [${sim2dev}-node0] (16 lines)"
	num_lines_is 1
}

test_detect_multiple_chips() {
	gpiosim_chip_dbus sim0 num_lines=4
	gpiosim_chip_dbus sim1 num_lines=8
	gpiosim_chip_dbus sim2 num_lines=16

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}
	local sim1=${GPIOSIM_CHIP_NAME[sim1]}
	local sim2=${GPIOSIM_CHIP_NAME[sim2]}
	local sim0dev=${GPIOSIM_DEV_NAME[sim0]}
	local sim1dev=${GPIOSIM_DEV_NAME[sim1]}
	local sim2dev=${GPIOSIM_DEV_NAME[sim2]}

	run_prog gpiocli detect $sim0 $sim1 $sim2

	status_is 0
	output_contains_line "$sim0 [${sim0dev}-node0] (4 lines)"
	output_contains_line "$sim1 [${sim1dev}-node0] (8 lines)"
	output_contains_line "$sim2 [${sim2dev}-node0] (16 lines)"
	num_lines_is 3
}

test_detect_with_nonexistent_chip() {
	run_prog gpiocli detect nonexistent_chip

	status_is 1
	output_regex_match ".*: No such chip: 'nonexistent_chip'"
}

#
# gpiocli info test cases
#

test_info_all_chips() {
	gpiosim_chip_dbus sim0 num_lines=4
	gpiosim_chip_dbus sim1 num_lines=8

	run_prog gpiocli info

	status_is 0
	output_contains_line "${GPIOSIM_CHIP_NAME[sim0]} - 4 lines:"
	output_contains_line "${GPIOSIM_CHIP_NAME[sim1]} - 8 lines:"
	output_regex_match "\\s+line\\s+0:\\s+unnamed\\s+\[input\]"
	output_regex_match "\\s+line\\s+7:\\s+unnamed\\s+\[input\]"
}

test_info_one_chip() {
	gpiosim_chip_dbus sim0 num_lines=4
	gpiosim_chip_dbus sim1 num_lines=8
	gpiosim_chip_dbus sim2 num_lines=12

	local sim1=${GPIOSIM_CHIP_NAME[sim1]}

	# by name
	run_prog gpiocli info -c $sim1

	status_is 0
	output_contains_line "$sim1 - 8 lines:"
	output_regex_match "\\s+line\\s+2:\\s+unnamed\\s+\[input\]"
	num_lines_is 9

	# by number
	run_prog gpiocli info -c $(gpiosim_chip_number sim1)

	status_is 0
	output_contains_line "$sim1 - 8 lines:"
	output_regex_match "\\s+line\\s+2:\\s+unnamed\\s+\[input\]"
	num_lines_is 9
}

test_info_one_line_by_name() {
	gpiosim_chip_dbus sim0 num_lines=8 line_name=3:foo line_name=5:bar
	gpiosim_chip_dbus sim1 num_lines=8 line_name=2:baz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli info bar

	status_is 0
	output_regex_match "$sim0\\s+5:\\s+\"bar\"\\s+\[input\]"
	num_lines_is 1
}

test_info_one_line_by_chip_and_offset() {
	gpiosim_chip_dbus sim0 num_lines=8
	gpiosim_chip_dbus sim1 num_lines=8

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli info -c $sim0 3

	status_is 0
	output_regex_match "$sim0\\s+3:\\s+unnamed\\s+\[input\]"
	num_lines_is 1
}

test_info_two_lines_by_chip_offset_and_name() {
	gpiosim_chip_dbus sim0 num_lines=8 line_name=3:foo line_name=5:bar
	gpiosim_chip_dbus sim1 num_lines=8 line_name=2:baz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli info -c $sim0 3 bar

	status_is 0
	output_regex_match "$sim0\\s+3:\\s+\"foo\"\\s+\[input\]"
	output_regex_match "$sim0\\s+5:\\s+\"bar\"\\s+\[input\]"
	num_lines_is 2
}

test_info_two_lines() {
	gpiosim_chip_dbus sim0 num_lines=8 line_name=3:foo line_name=5:bar
	gpiosim_chip_dbus sim1 num_lines=8 line_name=2:baz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}
	local sim1=${GPIOSIM_CHIP_NAME[sim1]}

	run_prog gpiocli info bar baz

	status_is 0
	output_regex_match "$sim0\\s+5:\\s+\"bar\"\\s+\[input\]"
	output_regex_match "$sim1\\s+2:\\s+\"baz\"\\s+\[input\]"
	num_lines_is 2
}

test_info_repeating_lines() {
	gpiosim_chip_dbus sim0 num_lines=8 line_name=3:foo line_name=5:bar
	gpiosim_chip_dbus sim1 num_lines=8 line_name=2:baz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}
	local sim1=${GPIOSIM_CHIP_NAME[sim1]}

	run_prog gpiocli info baz bar baz

	status_is 0
	output_regex_match "$sim1\\s+2:\\s+\"baz\"\\s+\[input\]"
	output_regex_match "$sim0\\s+5:\\s+\"bar\"\\s+\[input\]"
	output_regex_match "$sim1\\s+2:\\s+\"baz\"\\s+\[input\]"
	num_lines_is 3
}

#
# gpiocli map test cases
#

test_map_existing_line() {
	gpiosim_chip_dbus sim0 num_lines=4 line_name=3:baz
	gpiosim_chip_dbus sim1 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	run_prog gpiocli map bar

	status_is 0
	num_lines_is 1
	output_is "${GPIOSIM_CHIP_NAME[sim1]} 5"
}

test_map_nonexistent_line() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	run_prog gpiocli map foobar

	status_is 1
	num_lines_is 1
	output_regex_match ".*: line 'foobar' not found"
}

#
# gpiocli request test cases
#

test_request_invalid_arguments() {
	gpiosim_chip_dbus sim0 num_lines=8 line_name=3:foo

	run_prog gpiocli request --input --output foo
	status_is 1
	output_regex_match ".*: --input and --output are mutually exclusive"

	run_prog gpiocli request --output --both-edges foo
	status_is 1
	output_regex_match ".*: monitoring edges is only possible in input mode"

	run_prog gpiocli request --output --open-source --open-drain foo
	status_is 1
	output_regex_match ".*: --push-pull, --open-drain and --open-source are mutually exclusive"

	run_prog gpiocli request --input --open-source foo
	status_is 1
	output_regex_match ".*: --push-pull, --open-drain and --open-source are only available in output mode"

	run_prog gpiocli request --input --bias-disabled --pull-down foo
	status_is 1
	output_regex_match ".*: --pull-up, --pull-down and --bias-disabled are mutually exclusive"
}

test_request_one_line_by_name() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	run_prog gpiocli request --input bar
	status_is 0
	num_lines_is 1
	local request=$output

	gpiodbus_check_request "$request\\s+\(${GPIOSIM_CHIP_NAME[sim0]}\)\\s+Offsets:\\s+\[5\]"
	gpiodbus_release_request $request
}

test_request_one_line_by_chip_and_offset() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --chip=$sim0 4
	status_is 0
	num_lines_is 1
	local request=$output

	gpiodbus_check_request "$request\\s+\(${GPIOSIM_CHIP_NAME[sim0]}\)\\s+Offsets:\\s+\[4\]"
	gpiodbus_release_request $request
}

test_request_from_different_chips() {
	gpiosim_chip_dbus sim0 num_lines=8 line_name=1:foo line_name=5:bar
	gpiosim_chip_dbus sim1 num_lines=4 line_name=1:xyz

	run_prog gpiocli request --input foo xyz
	status_is 1
	output_regex_match ".*: all requested lines must belong to the same chip"
}

test_multiple_requests() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --chip=$sim0 0
	status_is 0
	num_lines_is 1
	local request0=$output

	run_prog gpiocli request --output --chip=$sim0 1 2
	status_is 0
	num_lines_is 1
	local request1=$output

	run_prog gpiocli request --chip=$sim0 5
	status_is 0
	num_lines_is 1
	local request2=$output

	run_prog gpiocli requests
	status_is 0
	num_lines_is 3
	output_contains_line "$request0 ($sim0) Offsets: [0]"
	output_contains_line "$request1 ($sim0) Offsets: [1, 2]"
	output_contains_line "$request2 ($sim0) Offsets: [5]"

	gpiodbus_release_request $request0
	gpiodbus_release_request $request1
	gpiodbus_release_request $request2
}

test_request_multiple_lines_by_names() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	run_prog gpiocli request --input foo xyz bar
	status_is 0
	num_lines_is 1
	local request=$output

	gpiodbus_check_request "$request\\s+\(${GPIOSIM_CHIP_NAME[sim0]}\)\\s+Offsets:\\s+\[1, 11, 5\]"
	gpiodbus_release_request $request
}

test_request_multiple_lines_by_chip_number_by_name_and_offset() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input -c $(gpiosim_chip_number sim0) xyz 0 foo 15
	status_is 0
	num_lines_is 1
	local request=$output

	gpiodbus_check_request "$request\\s+\(${GPIOSIM_CHIP_NAME[sim0]}\)\\s+Offsets:\\s+\[11, 0, 1, 15\]"
	gpiodbus_release_request $request
}

test_request_with_consumer_name() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --consumer='foobar' foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "gpiochip1\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"foobar\",managed=\"request0\",input\]"

	gpiodbus_release_request $request
}

test_request_with_consumer_name_with_whitespaces() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --consumer='foo bar' foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "gpiochip1\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"foo bar\",managed=\"request0\",input\]"

	gpiodbus_release_request $request
}

test_request_active_low() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --active-low foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",active-low,input\]"

	run_prog gpiocli get foo
	status_is 0
	num_lines_is 1
	output_is "\"foo\"=active"

	gpiodbus_release_request $request
}

test_request_pull_up() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --pull-up foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",bias=pull-up,input\]"

	run_prog gpiocli get foo
	status_is 0
	num_lines_is 1
	output_is "\"foo\"=active"

	gpiodbus_release_request $request
}

test_request_pull_down() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --pull-down foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",bias=pull-down,input\]"

	run_prog gpiocli get foo
	status_is 0
	num_lines_is 1
	output_is "\"foo\"=inactive"

	gpiodbus_release_request $request
}

test_request_pull_bias_disabled() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --bias-disabled foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",bias=disabled,input\]"

	run_prog gpiocli get foo
	status_is 0
	num_lines_is 1
	output_is "\"foo\"=inactive"

	gpiodbus_release_request $request
}

test_request_drive_push_pull() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --output --push-pull foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",output,push-pull\]"

	gpiodbus_release_request $request
}

test_request_drive_open_drain() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --output --open-drain foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",output,open-drain\]"

	gpiodbus_release_request $request
}

test_request_drive_open_source() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --output --open-source foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",output,open-source\]"

	gpiodbus_release_request $request
}

test_request_edge_falling() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --falling-edge foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",edges=falling,event_clock=monotonic,input\]"

	gpiodbus_release_request $request
}

test_request_edge_rising() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --rising-edge foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",edges=rising,event_clock=monotonic,input\]"

	gpiodbus_release_request $request
}

test_request_edge_both() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --both-edges foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",edges=both,event_clock=monotonic,input\]"

	gpiodbus_release_request $request
}

test_request_edge_falling_and_rising() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	local sim0=${GPIOSIM_CHIP_NAME[sim0]}

	run_prog gpiocli request --input --falling-edge --rising-edge foo
	status_is 0
	num_lines_is 1
	local request=$output

	run_prog gpiocli info foo
	status_is 0
	num_lines_is 1
	output_regex_match "$sim0\\s+1:\\s+\"foo\"\\s+\[used,consumer=\"gpiocli request\",managed=\"$request\",edges=both,event_clock=monotonic,input\]"

	gpiodbus_release_request $request
}

#
# gpiocli release test cases
#

test_release_nonexistent_request() {
	run_prog gpiocli release request0
	status_is 1
	output_regex_match ".*: No such request: 'request0'"
}

#
# gpiocli get test cases
#

test_get_one_value_by_name() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	run_prog gpiocli request --input foo
	status_is 0
	num_lines_is 1
	local request=$output

	gpiosim_set_pull sim0 1 pull-up

	run_prog gpiocli get foo
	status_is 0
	num_lines_is 1
	output_is "\"foo\"=active"

	gpiodbus_release_request $request
}

test_get_multiple_values_by_chip_and_offsets() {
	gpiosim_chip_dbus sim0 num_lines=16 line_name=1:foo line_name=5:bar line_name=11:xyz

	run_prog gpiocli request --input foo bar xyz
	status_is 0
	num_lines_is 1
	local request=$output

	gpiosim_set_pull sim0 1 pull-up
	gpiosim_set_pull sim0 5 pull-down
	gpiosim_set_pull sim0 11 pull-up

	run_prog gpiocli get --request=$request 11 1 5
	status_is 0
	num_lines_is 1
	output_is "\"11\"=active \"1\"=active \"5\"=inactive"

	gpiodbus_release_request $request
}

. gpiod-bash-test-helper.inc
check_prog gdbus
. shunit2
