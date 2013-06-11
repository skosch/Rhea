#ifndef CSVROW_H
#define CSVROW_H

/****************** CSV HANDLING ********************/

class CSVRow
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
   /* std::istream& operator<<(std::istream& str) {
      readNextRow(str);
      return str;
    } */
  private:
    std::vector<std::string>    m_data;
};
inline std::istream& operator<<(CSVRow& data, std::istream& str)
{
  data.readNextRow(str);
  return str;
} 


#endif
