
void assert_or_die(bool cond, std::string msg)
{
	if (!cond) {
		std::cerr << msg << std::endl;
		exit(-1);
	}
}
