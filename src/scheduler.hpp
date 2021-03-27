#pragma once

class scheduler
{
public:	
	static const bool cond_continue = false;
	static const bool cond_end = true;

	void schedule(std::function<bool()> callback, std::chrono::milliseconds delay = 0ms);
	void loop(const std::function<void()>& callback, std::chrono::milliseconds delay = 0ms);
	void once(const std::function<void()>& callback, std::chrono::milliseconds delay = 0ms);

	void run_frame();

private:
	struct task
	{
		std::function<bool()> handler;
		std::chrono::milliseconds interval{};
		std::chrono::high_resolution_clock::time_point last_call{};
	};
	
	std::recursive_mutex mutex_;
	volatile bool is_executing_ = false;
	std::vector<task> tasks_;
	std::vector<task> new_tasks_;

	void add_task(task&& task);
	void merge_new_tasks();
	void run_pending_tasks_internal();
};
