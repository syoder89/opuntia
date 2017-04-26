        static s64 elapsed = 0;
        struct timespec64 before, after, delta;
        static u32 count = 0;
        static u64 next_update = 0;

        getnstimeofday64(&before);

	// fn to profile

        getnstimeofday64(&after);
	// These getnstimeofday64 calls take 320ns total (160 each) on the Compex WPQ864
        delta = timespec64_sub(after, before);
        elapsed += timespec64_to_ns(&delta);
        count++;

        if (!next_update || time_after64(get_jiffies_64(), next_update)) {
                u64 m_elapsed = (u64)elapsed;
                do_div(m_elapsed,1000000),
                printk("ath10k napi_poll: %d calls, %lldms total (%lldns), last call %lldns\n", count, m_elapsed, elapsed, timespec64_to_ns(&delta));
                count = 0;
                elapsed = 0;
                next_update = get_jiffies_64() + (u64)msecs_to_jiffies(1000);
        }
