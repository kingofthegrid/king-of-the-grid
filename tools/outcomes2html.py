import json
import sys
from datetime import datetime


OUTCOMES_JSON = sys.argv[1]
OUTPUT_HTML = sys.argv[2]
PREFIX_PATH = sys.argv[3]

# Read the JSON data from outcome.json
with open(OUTCOMES_JSON, "r") as file:
    data = json.load(file)

date_s = str(datetime.now())

# Generate the HTML content
html_content = f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">
    <title>King Of The Grid Leaderboard</title>
    <link rel="stylesheet" href="bootstrap.min.css">
    <link rel="stylesheet" href="bootstrap-icons.min.css">
</head>
<body>
    <div class="container mt-5">
        <div id="game-header" class="text-center mb-4">
            <img src="kotg.png" width="128px"><br/>
        </div>
        <div class="card border-success">
            <div class="card-header">
                <span class="text-white">
                    King Of The Grid Leaderboard
                </span>
                <span class="d-flex float-right">({date_s})</span>
            </div>
            <div class="card-body">
                <table class="table table-dark">
                    <tr>
                        <th>Bot Name</th>
                        <th>Score</th>
                        <th>Wins</th>
                    </tr>
"""

# Generate table rows from the JSON data
for index, entry in enumerate(data):
    bot_name = entry["bot"]
    score = entry["score"]
    wins = entry["wins"]
    win_links = "<br/>".join([f'<a href="browser.html#{PREFIX_PATH}{win}" target="_blank">{win}</a>' for win in wins])

    html_content += f"""
                    <tr>
                        <td>{index+1}. {bot_name}</td>
                        <td>{score}</td>
                        <td>{win_links if wins else "No Wins"}</td>
                    </tr>
    """

# Close the HTML tags
html_content += """
                </table>
            </div>
        </div>
        <div class="text-center mt-4">
            <a href="run.html" class="btn btn-outline-info" role="button">Run Bots Live (Web)</a>
            <a href="https://github.com/kingofthegrid/bots" class="btn btn-outline-warning" role="button">Participate In Competition</a>
            <a href="https://github.com/kingofthegrid/king-of-the-grid?tab=readme-ov-file#game-rules" class="btn btn-outline-danger" role="button">Game Rules</a>
        </div>
    </div>
</body>
</html>
"""

# Save the HTML content to a file
with open(OUTPUT_HTML, "w") as file:
    file.write(html_content)

print("HTML page generated successfully as 'outcome.html'.")
