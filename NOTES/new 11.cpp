size_t swap_candidate_index{objects.size() - 1}; //TODO check if size is 0
auto last_objects_to_be_destroyed_it{objects_to_be_destroyed.rbegin()};

NEXT:
while (swap_candidate_index == last_objects_to_be_destroyed_it->index) { swap_candidate_index--; last_objects_to_be_destroyed_it++; }
if (objects_to_be_destroyed_it > last_objects_to_be_destroyed_it) { goto OUT; }

objects[objects_to_be_destroyed_it->index] = std::move(objects[swap_candidate_index]);

swap_candidate_index--;
objects_to_be_destroyed_it++;
goto NEXT;
OUT:
