enum class things_t { something, another_thing, something_else };


class Some_class
	{
	MAKE_ALIASED_ARRAY(things_t, float, 3)
	};
	
expands to:
	
class Some_class
	{
	union
		{
		struct
			{
			std::array<float, 3> data;
			};
		struct
			{
			float something;
			float another_thing;
			float something_else;
			}
		}
	};