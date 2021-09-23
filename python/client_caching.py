import datetime
import random
import time

class MyCache:
	""""""

	def __init__(self):
		"""Constructor"""
		self.cache = {}
		self.max_cache_size = 1024    # size of cache

	def __contains__(self, key):
		"""
		Returns True or False depending on whether or not the key is in the 
		cache
		"""
		return key in self.cache
	
	def add_cache(self, key_path, content, t_mserver):
		"""
		Add cache key (use file path name as key path)
		"""
		if key_path not in self.cache:
			self.cache[key_path] = {'time_validated': int(time.time()),
									't_mclient' : int(t_mserver),
									'content': content}	
			print("self.cache[key_path]: ",self.cache[key_path]['content'])
	
	def update(self, key_path, content, t_mserver):
		"""
		Update the cache dictionary 
		"""
#		if key not in self.cache and len(self.cache) >= self.max_cache_size:
#			self.remove_oldest()
		
		current_time = int(time.time())
		if key_path not in self.cache:
			self.add_cache(key_path, content, t_mserver)
				
		elif current_time >= self.cache[key_path]['time_validated']:
			self.cache[key_path] = {'time_validated': int(time.time()),
									't_mclient': int(t_mserver),
									'content': content}
		else:
			print("Content is not updates OR time_accessed went wrong!")
		
#	def remove_oldest(self):
#		"""
#		Remove the entry that has the oldest accessed date
#		"""
#		oldest_entry = None
#		for key in self.cache:
#			if oldest_entry is None:
#				oldest_entry = key
#			elif self.cache[key]['time_accessed'] < self.cache[oldest_entry][
#				'time_accessed']:
#				oldest_entry = key
#		self.cache.pop(oldest_entry)
	
	def size(self):
		"""
		Return the size of the cache
		"""
		return len(self.cache)
		
	def print_keys_existing(self):
		"""
		Return current keys existing in cache
		"""
		for key in self.cache:
			print(key)