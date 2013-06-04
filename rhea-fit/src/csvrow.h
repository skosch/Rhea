/****************** CSV HANDLING ********************/

class CVSRow
{
  public:
    std::string const& operator[](std::size_t index) const
    {
      return m_data[index];
    }
    std::size_t size() const
    {
      return m_data.size();
    }
    void readNextRow(std::istream& str)
    {
      std::string         line;
      std::getline(str,line);

      std::stringstream   lineStream(line);
      std::string         cell;

      m_data.clear();
      while(std::getline(lineStream,cell,','))
      {
        m_data.push_back(cell);
      }
    }
  private:
    std::vector<std::string>    m_data;
};
std::istream& operator>>(std::istream& str,CVSRow& data)
{
  data.readNextRow(str);
  return str;
}
